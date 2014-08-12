#define		_FLYETHERNETGOABLE_

#include "config.h"
#include "lpc17xx_lib.h"
#include "includes.h"
#include "UserType.h"

#include "tcpip.h"
#include "tcp.h"
#include "ip.h"
#include "Ethernetif.h"
#include "Etharp.h"



#define		NON_API		0
#define		RAW_API		1
#define		LWIP_API	2
#define		WHAT_API	LWIP_API

#define		APP_HTTP	0
#define		APP_FTP		1

#define		APP_WHAT	APP_FTP


#include "ftp.c"



BYTE ip_addr[] = {192,168,8,100};
BYTE gw_addr[] = {192, 168, 1, 1};
BYTE netmask[] = { 255, 255, 0, 0};	
BYTE mac_addr[] = {MAC0,MAC1,MAC2,MAC3,MAC4,MAC5};


/***************************************************************************************************************************
**函数名称:	 	ENET_IRQHandler
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void ENET_IRQHandler(void)
{
	//OSIntEnter();
	if(EMAC_IntGetStatus(EMAC_INT_RX_DONE))
	{
		if(NULL != flyEhternetInfo.LwIPSem)
		{
			OSSemPost(flyEhternetInfo.LwIPSem);
		}
	}
	
	LPC_EMAC->IntClear = LPC_EMAC->IntStatus;
	//OSIntExit();
}
/***************************************************************************************************************************
**函数名称:	 	ipcEventProcFlylyEthernet
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
static err_t client_connected(void *arg, struct tcp_pcb *pcb, err_t err)
{
	LIBMCU_DEBUG(ETHERNTE_DEBUG,("\r\n client_connected"));
	return ERR_OK;
}
/***************************************************************************************************************************
**函数名称:	 	LwIP_TimeOutTask
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void LwIP_TimeOutTask(void *arg)
{
	LIBMCU_DEBUG(ETHERNTE_DEBUG,("\r\n LwIP_TimeOutTask %d ", OSTimeGet()));
}

/***************************************************************************************************************************
**函数名称:	 	tcp_recv_back
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
err_t tcp_recv_back(void *arg, struct tcp_pcb *tpcb,struct pbuf *p, err_t err)
{
	UINT16 len;
	LIBMCU_DEBUG(ETHERNTE_DEBUG,("\r\n tcp_recv_back"));
	
	len = p->len + p->tot_len;
	
	LIBMCU_DEBUG(ETHERNTE_DEBUG,("\r\n len = %d ",len));
	pbuf_free(p);
	tcp_close(tpcb);
	return ERR_OK;
}
/***************************************************************************************************************************
**函数名称:	 	LwIP_Accept
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
err_t LwIP_Accept(void *arg, struct tcp_pcb *newpcb, err_t err)
{
	LIBMCU_DEBUG(ETHERNTE_DEBUG,("\r\n LwIP_Accept"));
	tcp_setprio(newpcb,TCP_PRIO_MIN);
	tcp_recv(newpcb,tcp_recv_back);
	return ERR_OK;
}
/***************************************************************************************************************************
**函数名称:	 	ip_input
**函数功能:	 	
				eth_hdr:
				+----------------------------------------------------------------------------------------+
				+  desMAC[6]  |  srcMAC[6]  |  type[2]( ARP / IP)  |  	数据[46-1500]   |	校验[4]      +
				+----------------------------------------------------------------------------------------+
				*注: 
					desMAC[6]: 目的MAC地址;
					srcMAC[6]: 源MAC地址;
					type[2]: 类型(IP:0X800/ARP:0X806)
					数据[46-1500]:具体数据;
					校验[4]:校验;


				etharp_hdr:
				+-----------------------------------------------------------------------------------------------------+
				+ hwhdr[2] | porotl[2] | hwlen[1] | porotllen[1] | op[2] | srcMAC[6] | srcIP[4] |desMAC[6] | desIP[4] +
				+-----------------------------------------------------------------------------------------------------+
				*注: 
					hwhdr[2]:硬件类型字段-表示硬件地址的类型,它的值为1即表示以太网MAC地址,长度为6个字节;
					porotl[2]:协议类型字段表示要映射的协议地址类型,它的值为0X800即表示要映射的IP地址;
					hwlen[1]: 硬件地址长度,对于以太网ARP请求或者应答来说,他的值为6;
					porotllen[1]:协议地址长度,对于以太网ARP请求或者应答来说,他的值为4;
					op[2]:操作类型 ARP请求=1,ARP应答=2,RARP请求=3,RARP=4;
					srcMAC[6]:发送端MAC地址;
					srcIP[4]: 发送端IP地址;
					desMAC[6]:目的端MAC地址
					desIP[4]: 目的端IP地址;
				ip_hdr:
				+---------------------------------------------------------------------------------------------------------------------------------------------------------------------+
				+ v(bit4) | hdrlen(bit4) | TOS[1] | len[2] | flag0[2] | flag1(bit3) | offset(bit13) | ttl[1] | porotl[1] |  hdrchecsum[2] | srcIP[4] | desIP[4] | 选项[...] | data[N] +																								  +
				+---------------------------------------------------------------------------------------------------------------------------------------------------------------------+
				*注: 
				v(bit4): 版本号,目前的版本号是4即IPv4;
				hdrlen(bit4): 首部长度-指首部站32Bit字的数目,包括可选项,由于它是个4bit因此首部最长60字节;
				TOS[1]:包括一个3bit的优先字段,4bit TOS字段和1 bit未使用但必须为0,如果所有4bit都为0,即一般服务;
				len[2]:整个IP数据报的长度以字节为单位;
				flag0[2]:指唯一标示主机发送的每一份数据报,通常每发送一份报文值就会加1;
				flag1(bit3): 用于在IP数据包分片时使用;
				offset(bit13):  用于在IP数据包分片时使用;
				ttl[1]:描述该IP数据包最多能被转发多少次,每经过一次转发,该值就会减1,当该值为0时,一个ICMP报文会被返回至源主机;
				porotl[1]: 协议字段描述该IP数据包是来自上层哪个协议,改值为1表示为ICMP,2=IGMP,6=TCP,17=UDP;
				hdrchecsum[2]:首部校验和只针对IP首部做校验,它并不关心其内部数据在传输过程中出错与否,对于数据校验是上层协议负责的;
				srcIP[4]:源IP地址;
				desIP[4]:目标IP地址;
				选项[...]:选项,可有可无;
				data[N]:数据data;
				
				icmp_hdr:
				+--------------------------------------------------------------------------------+
				+ type[1] | code[1] | checksum[2] | 	others[N](不同的类型和代码有不同的内容)  |
				+--------------------------------------------------------------------------------+
				
				
**入口参数:
**返回参数:
***************************************************************************************************************************/
void ipcFlyEthernetInit(void)
{
	struct ip_addr ip_computer;
	memset(&flyEhternetInfo,0,sizeof(flyEhternetInfo));
	
	flyEhternetInfo.LwIPSem = OSSemCreate(0);
	if(NULL == flyEhternetInfo.LwIPSem)
	{
		LIBMCU_DEBUG(ETHERNTE_DEBUG,("\r\n OSSemCreate Fail"));
	}
	
	tcpip_init(NULL,NULL);
	
	IP4_ADDR(&flyEhternetInfo.ipaddr,  192, 168, 8, 7);
	IP4_ADDR(&flyEhternetInfo.netmask, 255, 255, 0, 0);
	IP4_ADDR(&flyEhternetInfo.gw, 192, 168, 1, 1);
	
	
	netif_add(&flyEhternetInfo.netif, &flyEhternetInfo.ipaddr, &flyEhternetInfo.netmask, &flyEhternetInfo.gw, NULL, ethernetif_init, ethernet_input);
	netif_set_default(&flyEhternetInfo.netif);
	netif_set_up(&flyEhternetInfo.netif);
	FlyEthernetCreate();
	LwipTaskCreate();
	EMAC_IntStart();
//	OpenMMCFile("mmc:\\http\\baidu.txt");
}
/***************************************************************************************************************************
**函数名称:	 	ipcEventProcFlylyEthernet
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void ipcEventProcFlylyEthernet(ULONG enumWhatEvent,ULONG lPara,BYTE *p,uint8_t length)
{
	BOOL bRes = TRUE;
	switch(enumWhatEvent)
	{
		case EVENT_GLOBAL_MODULE_INIT:		ipcFlyEthernetInit();				
											break;
		
		default:							break;
	}
	
	if(bRes)
	{
		ipcClearEvent(enumWhatEvent);
	}
}
/***************************************************************************************************************************
**函数名称:	 	ipcEventProcFlylyEthernet
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void lwipServiceInit(void)
{
	BYTE port = 0;
	#if(APP_HTTP == APP_WHAT)
	port = 80;
	#elif(APP_FTP == APP_WHAT)
	ftpd_init();
	port = 21;
	#endif
	
	
	#if(RAW_API == WHAT_API)
	flyEhternetInfo.pTcp = tcp_new();
	if(NULL == flyEhternetInfo.pTcp)
	{
		LIBMCU_DEBUG(ETHERNTE_DEBUG,("\r\n tcp_new fail"));
	}
	
	if(ERR_OK != tcp_bind(flyEhternetInfo.pTcp,IP_ADDR_ANY,port))//IP_ADDR_ANY
	{
		LIBMCU_DEBUG(ETHERNTE_DEBUG,("\r\n tcp_bind fail"));
	}
	
	flyEhternetInfo.pTcp = tcp_listen(flyEhternetInfo.pTcp);
	
	tcp_accept(flyEhternetInfo.pTcp,LwIP_Accept);
	#elif(LWIP_API == WHAT_API)
	flyEhternetInfo.pNetconn = netconn_new(NETCONN_TCP);
	if(NULL == flyEhternetInfo.pNetconn)
	{
		LIBMCU_DEBUG(ETHERNTE_DEBUG,("\r\n netconn_new fail"));
	}
	
	if(ERR_OK != netconn_bind(flyEhternetInfo.pNetconn,IP_ADDR_ANY,port))
	{
		LIBMCU_DEBUG(ETHERNTE_DEBUG,("\r\n netconn_bind fail"));
	}
	
	if(ERR_OK != netconn_listen(flyEhternetInfo.pNetconn))
	{
		LIBMCU_DEBUG(ETHERNTE_DEBUG,("\r\n netconn_listen fail"));
	}
	
	#endif
}
/***************************************************************************************************************************
**函数名称:	 	FlyEthernetTask
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void FlyEthernetTask(void *arg)
{
	INT8U err;
	while(1)
	{
		OSSemPend(flyEhternetInfo.LwIPSem,500,&err);
		if((OS_NO_ERR != err) && (OS_TIMEOUT != err))
		{
			LIBMCU_DEBUG(ETHERNTE_DEBUG,("\r\n err = %d ",err));
		}
		ethernetif_input(&flyEhternetInfo.netif);
	}
}
/***************************************************************************************************************************
**函数名称:	 	FlyEthernetCreate
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void FlyEthernetCreate(void)
{
	INT8U Res;
	Res = OSTaskCreate(FlyEthernetTask, 								//执行函数
				 NULL,	  												//带入的参数
				 &GstkFlyEthernet[FLYETHERNET_TASK_START_STK_SIZE-1],	//堆栈由高地址往底地址增长
				 (INT8U)PRIO_FLYEHTERNET								//任务优先级
				 );
	if(OS_NO_ERR != Res)
	{
		LIBMCU_DEBUG(ETHERNTE_DEBUG,("\r\n Res = %d",Res));
	}
}
/***************************************************************************************************************************
**函数名称:	 	OpenMMCFile
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
BOOL OpenMMCFile(char *name)
{	
#if 1
	flyEhternetInfo.httpfd = FS_FOpen(name,"r");
	if(NULL == flyEhternetInfo.httpfd)
	{
		LIBMCU_DEBUG(FILE_DEBUG,("\r\n FS_FOpen Fail"));
		return FALSE;
	}
#endif	
	return TRUE;
}
/***************************************************************************************************************************
**函数名称:	 	HttpsServicer
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void HttpsServicer(char *p,UINT len)
{
	UINT16 i = 0;
	UINT16 copycount = 0;
	LIBMCU_DEBUG(ETHERNTE_DEBUG,("\r\n len = %d ",len));
	for(i = 0;i < len;i++)
	{
		LIBMCU_DEBUG(ETHERNTE_DEBUG,("%c",p[i]));
	}
	
	if ((len >= 5) && (strncmp(p, "GET /", 5) == 0))
	{
		if(OpenMMCFile("mmc:\\http\\baidu.txt"))
		{
			LIBMCU_DEBUG(ETHERNTE_DEBUG,("\r\n Open baidu.htm OK size = %d ",flyEhternetInfo.httpfd->size));
			copycount = flyEhternetInfo.httpfd->size / RECSIZE;
			for(i  = 0;i < copycount;i++)
			{
				FS_FRead(flyEhternetInfo.recBuf,1,RECSIZE,flyEhternetInfo.httpfd);
				netconn_write(flyEhternetInfo.pNewnetconn,(void *)flyEhternetInfo.recBuf,RECSIZE,NETCONN_NOCOPY);
			}
			
			if(flyEhternetInfo.httpfd->size % RECSIZE)
			{
				FS_FRead(flyEhternetInfo.recBuf,1,flyEhternetInfo.httpfd->size % RECSIZE,flyEhternetInfo.httpfd);
				netconn_write(flyEhternetInfo.pNewnetconn,(void *)flyEhternetInfo.recBuf,flyEhternetInfo.httpfd->size % RECSIZE,NETCONN_NOCOPY);
			}
			FS_FClose(flyEhternetInfo.httpfd);
		}
	}
}
/***************************************************************************************************************************
**函数名称:	 	fptWriteData
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void fptWriteData(struct netconn *ftpconn,char *p,UINT len)
{
	UINT i = 0;
	LIBMCU_DEBUG(ETHERNTE_DEBUG,("\r\n len: %d ",len));

	if(len)
	{
		LIBMCU_DEBUG(ETHERNTE_DEBUG,("\r\n fptWriteData: "));
		for(i = 0;i < len;i++)
		{
			LIBMCU_DEBUG(ETHERNTE_DEBUG,("%c ",p[i]));
		}	
		
		if(ERR_OK != netconn_write(ftpconn,(void *)p,len,NETCONN_NOCOPY))
		{
			LIBMCU_DEBUG(ETHERNTE_DEBUG,("\r\n fptWriteData  fail"));
		}
	}
}

/***************************************************************************************************************************
**函数名称:	 	fptServicer
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void fptServicer(void)
{
	char *ch = NULL;
	BOOL bRec = TRUE;
	UINT16 len;
	UINT16 i = 0;
	BYTE state = getFTPMsgState();
	LIBMCU_DEBUG(ETHERNTE_DEBUG,("\r\n fptServicer state = %d ",state));
	
	switch(state)
	{
		case FTPD_USER:		setFTPMsgState(FTPD_IDLE);
							msg_weclome(flyEhternetInfo.pNewnetconn);
							bRec = TRUE;
							break;
		case FTPD_PASS:		
							break;
		
		case FTPD_IDLE:		bRec = TRUE;
							break;
		case FTPD_NLST:		
							break;
		case FTPD_LIST:		
							break;
		case FTPD_RETR:		
							break;
		case FTPD_RNFR:		
							break;
		case FTPD_STOR:		
							break;
		case FTPD_QUIT:		
							break;
		default:			
							break;
						
	}
	
	if(bRec)
	{
		if(ERR_OK == netconn_recv(flyEhternetInfo.pNewnetconn,&flyEhternetInfo.pNetbuf))
		{
			LIBMCU_DEBUG(ETHERNTE_DEBUG,("\r\n xxoo netconn_recv msg"));
			netbuf_data(flyEhternetInfo.pNetbuf,(void *)&ch,&len);
			LIBMCU_DEBUG(ETHERNTE_DEBUG,("\r\n netbuf_data: "));
			for(i = 0;i < len;i++)
			{
				LIBMCU_DEBUG(ETHERNTE_DEBUG,("%c ",ch[i]));
			}
			ftpd_msgrecv(flyEhternetInfo.pNewnetconn,ch,len);
		}
		
		if(flyEhternetInfo.pNetbuf)
		{
			netbuf_delete(flyEhternetInfo.pNetbuf);    
		}
	}
}
/***************************************************************************************************************************
**函数名称:	 	LwipTaskCreate
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void LwipTask(void *arg)
{
	char *ch = NULL;
	UINT16 len = 0;
	UINT16 i = 0;
	err_t err;
	lwipServiceInit();
	err = netconn_accept(flyEhternetInfo.pNetconn,&flyEhternetInfo.pNewnetconn);
	if(ERR_OK != err)
	{
		LIBMCU_DEBUG(ETHERNTE_DEBUG,("\r\n netconn_accept fail"));
	}
	while(1)
	{
		#if(LWIP_API == WHAT_API)
			
			#if(APP_HTTP == APP_WHAT)
				if(ERR_OK == netconn_recv(flyEhternetInfo.pNewnetconn,&flyEhternetInfo.pNetbuf))
				{
					LIBMCU_DEBUG(ETHERNTE_DEBUG,("\r\n netconn_recv msg"));
					netbuf_data(flyEhternetInfo.pNetbuf,(void *)&ch,&len);
					HttpsServicer(ch,len);	
					netconn_close(conn);
					netbuf_delete(flyEhternetInfo.pNetbuf);   
					netconn_delete(flyEhternetInfo.pNewnetconn);
				}
			#elif(APP_FTP == APP_WHAT)
				if(ERR_OK == err)
				{
					fptServicer();
					LIBMCU_DEBUG(ETHERNTE_DEBUG,("\r\n netconn_accept OK"));
				}	
			#endif
		#else
			LIBMCU_DEBUG(ETHERNTE_DEBUG,("\r\n LwipTask"));
			OSTimeDly(OS_TICKS_PER_SEC );  
		#endif
	}
	if(1)//flyEhternetInfo.pNewnetconn
	{
		netconn_close(flyEhternetInfo.pNewnetconn);
		netconn_delete(flyEhternetInfo.pNewnetconn);
		flyEhternetInfo.pNewnetconn = NULL;
	}
}
/***************************************************************************************************************************
**函数名称:	 	LwipTaskCreate
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void LwipTaskCreate(void)
{
	INT8U Res;
	Res = OSTaskCreate(LwipTask, 										//执行函数
				 NULL,	  												//带入的参数
				 &GstkLwip[LWIP_TASK_START_STK_SIZE-1],					//堆栈由高地址往底地址增长
				 (INT8U)PRIO_LWIP										//任务优先级
				 );
	if(OS_NO_ERR != Res)
	{
		LIBMCU_DEBUG(ETHERNTE_DEBUG,("\r\n Res = %d",Res));
	}	
}
	
