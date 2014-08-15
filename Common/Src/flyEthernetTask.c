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

#define		APP_HTTP	0
#define		APP_FTP		1
#define		APP_WHAT	APP_FTP

#define		NON_API		0
#define		RAW_API		1
#define		LWIP_API	2
#define		WHAT_API	RAW_API


#if(APP_FTP == APP_WHAT)


enum Attri
{
	eNON = 0,
	eRAW,
	eLWIP
};

enum CtrMsg
{
	eCtr = 0,
	eMsg
};

typedef struct
{
	BYTE attri;
	struct tcp_pcb *ftpRAWControl;
	struct tcp_pcb *ftpRAWDataMsg;
	BYTE ctrOrMsg;
	
	tcp_connected_fn connected;
	tcp_accept_fn accept;
	tcp_poll_fn poll;
	tcp_recv_fn recv;
	tcp_sent_fn sent;
}FTP_INFO;


typedef void (*_p_ftp_arg)(FTP_INFO *pftpInfo,void *arg);
typedef void (*_p_ftp_accept)(FTP_INFO *pftpInfo);
typedef void (*_p_ftp_new)(FTP_INFO *pftpInfo);
typedef void (*_p_ftp_bind)(FTP_INFO *pftpInfo,UINT16 port);
typedef void (*_p_ftp_listen)(FTP_INFO *pftpInfo);
typedef void (*_p_ftp_connect)(FTP_INFO *pftpInfo,struct ip_addr dataip,UINT16 port);
typedef void (*_p_ftp_writeData)(FTP_INFO *pftpInfo,char *p,UINT16 len);
typedef void (*_p_ftp_in)(char *p,UINT16 len);

typedef struct
{
	_p_ftp_new  p_ftp_new;
	_p_ftp_bind p_ftp_bind;
	_p_ftp_listen p_ftp_listen;
	_p_ftp_connect p_ftp_connect;
	_p_ftp_accept p_ftp_accept;
	_p_ftp_writeData p_ftp_writeData;
	_p_ftp_arg p_ftp_arg;
}FTP_FUNC;


FTP_INFO ftp_info;
FTP_FUNC ftp_func;

#include "ftp.c"
#endif


BYTE ip_addr[] = {192,168,8,100};
BYTE gw_addr[] = {192, 168, 1, 1};
BYTE netmask[] = { 255, 255, 0, 0};	
BYTE mac_addr[] = {MAC0,MAC1,MAC2,MAC3,MAC4,MAC5};

err_t ftp_connected_fn(void *arg, struct tcp_pcb *tpcb, err_t err);
err_t ftp_poll_fn(void *arg, struct tcp_pcb *tpcb);
err_t ftp_sent_fn(void *arg, struct tcp_pcb *tpcb,u16_t len);
void ftp_arg(FTP_INFO *pftpInfo,void *arg);
void ftp_recv(FTP_INFO *pftpInfo);
void ftp_poll(FTP_INFO *pftpInfo,BYTE interval);
void ftp_sent(FTP_INFO *pftpInfo);
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


#if(APP_FTP == APP_WHAT)
/***************************************************************************************************************************
**函数名称:	 	ftp_recv_fn
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
err_t ftp_recv_fn(void *arg, struct tcp_pcb *tpcb,struct pbuf *p, err_t err)
{
	UINT16 len;
	UINT16 i;
	struct pbuf *q;
	static char recBuf[512];
	char *pt = recBuf;
	_p_ftp_in p_ftp_in;
	p_ftp_in = (_p_ftp_in)arg;
	LIBMCU_DEBUG(ETHERNTE_DEBUG,("\r\n tcp_recv_back"));
	for(q = p; q != NULL; q = q->next) 
	{
		memcpy(pt, q->payload, q->len);
		pt += q->len;
	}
	len = p->len + p->tot_len;	
	pt = recBuf;
	if(len >= 512)
	{
		len = 512;
	}
	LIBMCU_DEBUG(ETHERNTE_DEBUG,("\r\n len = %d ",len));
	for(i = 0;i < len;i++)
	{
		LIBMCU_DEBUG(ETHERNTE_DEBUG,("\r\n %c ",pt[i]));
	}
	p_ftp_in(pt,len);
	pbuf_free(p);
	return ERR_OK;
}
/***************************************************************************************************************************
**函数名称:	 	ftp_accept_fn
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
err_t ftp_accept_fn(void *arg, struct tcp_pcb *newpcb, err_t err)
{
	LIBMCU_DEBUG(ETHERNTE_DEBUG,("\r\n ftp_accept_fn"));	
	ftp_info.ftpRAWControl = newpcb;
	ftp_info.poll = ftp_poll_fn;
	ftp_info.recv = ftp_recv_fn;
	ftp_info.sent = ftp_sent_fn;
	ftp_info.connected = ftp_connected_fn;
	
	ftp_arg(&ftp_info,(void *)ftp_in);
	ftp_recv(&ftp_info);
	ftp_poll(&ftp_info,1);
	ftp_sent(&ftp_info);
	send_msg(msg220);
	return ERR_OK;
}

/***************************************************************************************************************************
**函数名称:	 	ftp_sent_fn
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
err_t ftp_sent_fn(void *arg, struct tcp_pcb *tpcb,u16_t len)
{
	return ERR_OK;
}

/***************************************************************************************************************************
**函数名称:	 	fpt_new
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void ftp_new(FTP_INFO *pftpInfo)
{
	if(eRAW == pftpInfo->attri)
	{
		if(eCtr == pftpInfo->ctrOrMsg)
		{
			pftpInfo->ftpRAWControl = tcp_new();
			LIBMCU_DEBUG(ETHERNTE_DEBUG,("\r\n ftpRAWControl %p",pftpInfo->ftpRAWControl));
			if(NULL == pftpInfo->ftpRAWControl)
			{
				LIBMCU_DEBUG(ETHERNTE_DEBUG,("\r\n Control tcp_new Fail"));
			}
		}
		else if(eMsg == pftpInfo->ctrOrMsg)
		{
			pftpInfo->ftpRAWDataMsg = tcp_new();
			LIBMCU_DEBUG(ETHERNTE_DEBUG,("\r\n ftpRAWDataMsg %p",pftpInfo->ftpRAWDataMsg));
			if(NULL == pftpInfo->ftpRAWDataMsg)
			{
				LIBMCU_DEBUG(ETHERNTE_DEBUG,("\r\n DataMsg tcp_new Fail"));
			}
		}
	}
	else if(eLWIP == pftpInfo->attri)
	{
	
	}
}
/***************************************************************************************************************************
**函数名称:	 	fpt_bind
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void ftp_bind(FTP_INFO *pftpInfo,UINT16 port)
{
	struct tcp_pcb *pcb;
	struct netconn *net_conn;
	if(eRAW == pftpInfo->attri)
	{
		if(eCtr == pftpInfo->ctrOrMsg)
		{
			if(ERR_OK != tcp_bind(pftpInfo->ftpRAWControl,IP_ADDR_ANY,port))//IP_ADDR_ANY
			{
				LIBMCU_DEBUG(ETHERNTE_DEBUG,("\r\n Control tcp_bind fail"));
			}
		}
		else if(eMsg == pftpInfo->ctrOrMsg)
		{
			if(ERR_OK != tcp_bind(pftpInfo->ftpRAWDataMsg,IP_ADDR_ANY,port))//IP_ADDR_ANY
			{
				LIBMCU_DEBUG(ETHERNTE_DEBUG,("\r\n DataMsg tcp_bind fail"));
			}
		}		
	}
	else if(eRAW == pftpInfo->attri)
	{
	
	}
}
/***************************************************************************************************************************
**函数名称:	 	fpt_listen
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void ftp_listen(FTP_INFO *pftpInfo)
{
	if(eRAW == pftpInfo->attri)
	{
		if(eCtr == pftpInfo->ctrOrMsg)
		{
			pftpInfo->ftpRAWControl = tcp_listen(pftpInfo->ftpRAWControl);
			LIBMCU_DEBUG(ETHERNTE_DEBUG,("\r\n new ftpRAWControl %p",pftpInfo->ftpRAWControl));
			if(NULL == pftpInfo->ftpRAWControl)
			{
				LIBMCU_DEBUG(ETHERNTE_DEBUG,("\r\n Control tcp_listen fail"));
			}
		}
		else if(eMsg == pftpInfo->ctrOrMsg)
		{
			pftpInfo->ftpRAWDataMsg = tcp_listen(pftpInfo->ftpRAWDataMsg);
			if(NULL == pftpInfo->ftpRAWDataMsg)
			{
				LIBMCU_DEBUG(ETHERNTE_DEBUG,("\r\n DataMsg tcp_listen fail"));
			}
		}
	}
	else if(eRAW == pftpInfo->attri)
	{
	
	}
}
/***************************************************************************************************************************
**函数名称:	 	fpt_accpet
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void ftp_accpet(FTP_INFO *pftpInfo)
{
	if(eRAW == pftpInfo->attri)
	{
		if(eCtr == pftpInfo->ctrOrMsg)
		{
			tcp_accept(pftpInfo->ftpRAWControl,pftpInfo->accept);
		}
		else if(eMsg == pftpInfo->ctrOrMsg)
		{
			tcp_accept(pftpInfo->ftpRAWDataMsg,pftpInfo->accept);
		}
	}
	else if(eLWIP == pftpInfo->attri)
	{
	
	}
}
/***************************************************************************************************************************
**函数名称:	 	fpt_connect
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void ftp_connect(FTP_INFO *pftpInfo,struct ip_addr dataip,UINT16 port)
{
	struct tcp_pcb *pcb;
	struct netconn *net_conn;
	if(eRAW == pftpInfo->attri)
	{
		if(eCtr == pftpInfo->ctrOrMsg)
		{
			if(ERR_OK != tcp_connect(pftpInfo->ftpRAWControl,&dataip,port,pftpInfo->connected))
			{
				LIBMCU_DEBUG(ETHERNTE_DEBUG,("\r\n Control tcp_connect fail"));
			}
		}
		else if(eMsg == pftpInfo->ctrOrMsg)
		{
			if(ERR_OK != tcp_connect(pftpInfo->ftpRAWDataMsg,&dataip,port,pftpInfo->connected))
			{
				LIBMCU_DEBUG(ETHERNTE_DEBUG,("\r\n DataMsg tcp_connect fail"));
			}
		}	
	}
	else if(eLWIP == pftpInfo->attri)
	{
	
	}
}
/***************************************************************************************************************************
**函数名称:	 	ftp_arg
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void ftp_arg(FTP_INFO *pftpInfo,void *arg)
{
	if(eRAW == pftpInfo->attri)
	{
		if(eCtr == pftpInfo->ctrOrMsg)
		{
			tcp_arg(pftpInfo->ftpRAWControl,arg);
		}
		else if(eMsg == pftpInfo->ctrOrMsg)
		{
			tcp_arg(pftpInfo->ftpRAWDataMsg,arg);
		}
	}
}
/***************************************************************************************************************************
**函数名称:	 	ftp_poll
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void ftp_poll(FTP_INFO *pftpInfo,BYTE interval)
{
	if(eRAW == pftpInfo->attri)
	{
		if(eCtr == pftpInfo->ctrOrMsg)
		{
			tcp_poll(pftpInfo->ftpRAWControl,pftpInfo->poll,interval);
		}
		else if(eMsg == pftpInfo->ctrOrMsg)
		{
			tcp_poll(pftpInfo->ftpRAWDataMsg,pftpInfo->poll,interval);
		}
	}
}
/***************************************************************************************************************************
**函数名称:	 	ftp_recv
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void ftp_recv(FTP_INFO *pftpInfo)
{
	if(eRAW == pftpInfo->attri)
	{
		if(eCtr == pftpInfo->ctrOrMsg)
		{
			tcp_recv(pftpInfo->ftpRAWControl,pftpInfo->recv);
		}
		else if(eMsg == pftpInfo->ctrOrMsg)
		{
			tcp_recv(pftpInfo->ftpRAWDataMsg,pftpInfo->recv);
		}
	}
}
/***************************************************************************************************************************
**函数名称:	 	ftp_sent
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void ftp_sent(FTP_INFO *pftpInfo)
{
	if(eRAW == pftpInfo->attri)
	{
		if(eCtr == pftpInfo->ctrOrMsg)
		{
			tcp_sent(pftpInfo->ftpRAWControl,pftpInfo->sent);
		}
		else if(eMsg == pftpInfo->ctrOrMsg)
		{
			tcp_sent(pftpInfo->ftpRAWDataMsg,pftpInfo->sent);
		}
	}
}
/***************************************************************************************************************************
**函数名称:	 	fptWriteData
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void ftp_writeData(FTP_INFO *pftpInfo,char *p,UINT16 len)
{
	UINT16 i = 0;
	LIBMCU_DEBUG(ETHERNTE_DEBUG,("\r\n ftp_writeData"));
	for(i = 0;i < len;i++)
	{
		LIBMCU_DEBUG(ETHERNTE_DEBUG,("%c",p[i]));
	}
	
	if(eRAW == pftpInfo->attri)
	{
		if(eCtr == pftpInfo->ctrOrMsg)
		{
			tcp_write(pftpInfo->ftpRAWControl,(void *)p,len,1);
		}
		else if(eMsg == pftpInfo->ctrOrMsg)
		{
			tcp_write(pftpInfo->ftpRAWDataMsg,(void *)p,len,1);
		}
	}
}
#endif
/***************************************************************************************************************************
**函数名称:	 	OpenMMCFile
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
BOOL OpenMMCFile(char *name)
{	
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

}
/***************************************************************************************************************************
**函数名称:	 	fptServicer
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void ftpServicer(void)
{
	
}
/***************************************************************************************************************************
**函数名称:	 	lwipServiceInit
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void lwipServiceInit(void)
{
#if(APP_FTP == APP_WHAT)
	#if(LWIP_API == WHAT_API)
	ftp_info.attri = eLWIP;
	#elif(RAW_API == WHAT_API)
	
	ftp_func.p_ftp_accept = ftp_accpet;
	ftp_func.p_ftp_bind = ftp_bind;
	ftp_func.p_ftp_connect = ftp_connect;
	ftp_func.p_ftp_listen = ftp_listen;
	ftp_func.p_ftp_new = ftp_new;
	ftp_func.p_ftp_writeData = ftp_writeData;
	ftp_func.p_ftp_arg = ftp_arg;
	
	ftp_info.attri = eRAW;
	ftp_info.ctrOrMsg = eCtr;
	ftp_info.accept = ftp_accept_fn;
	
	ftp_new(&ftp_info);
	ftp_bind(&ftp_info,21);
	ftp_listen(&ftp_info);
	ftp_accpet(&ftp_info);
	
	ftpd_init(&ftp_info,&ftp_func);
	#else
	
	#endif
#else
	
#endif	
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
	
	while(1)
	{
		#if(APP_FTP == APP_WHAT)
			#if(LWIP_API == WHAT_API)
			
			#elif(RAW_API == WHAT_API)
			OSTimeDly(OS_TICKS_PER_SEC / 2);  
			#endif
		#else
	
		#endif	
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
	
