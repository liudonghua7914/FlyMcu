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
**��������:	 	ENET_IRQHandler
**��������:	 	
**��ڲ���:
**���ز���:
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
**��������:	 	ipcEventProcFlylyEthernet
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
static err_t client_connected(void *arg, struct tcp_pcb *pcb, err_t err)
{
	LIBMCU_DEBUG(ETHERNTE_DEBUG,("\r\n client_connected"));
	return ERR_OK;
}
/***************************************************************************************************************************
**��������:	 	LwIP_TimeOutTask
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
void LwIP_TimeOutTask(void *arg)
{
	LIBMCU_DEBUG(ETHERNTE_DEBUG,("\r\n LwIP_TimeOutTask %d ", OSTimeGet()));
}

/***************************************************************************************************************************
**��������:	 	tcp_recv_back
**��������:	 	
**��ڲ���:
**���ز���:
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
**��������:	 	LwIP_Accept
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
err_t LwIP_Accept(void *arg, struct tcp_pcb *newpcb, err_t err)
{
	LIBMCU_DEBUG(ETHERNTE_DEBUG,("\r\n LwIP_Accept"));
	tcp_setprio(newpcb,TCP_PRIO_MIN);
	tcp_recv(newpcb,tcp_recv_back);
	return ERR_OK;
}
/***************************************************************************************************************************
**��������:	 	ip_input
**��������:	 	
				eth_hdr:
				+----------------------------------------------------------------------------------------+
				+  desMAC[6]  |  srcMAC[6]  |  type[2]( ARP / IP)  |  	����[46-1500]   |	У��[4]      +
				+----------------------------------------------------------------------------------------+
				*ע: 
					desMAC[6]: Ŀ��MAC��ַ;
					srcMAC[6]: ԴMAC��ַ;
					type[2]: ����(IP:0X800/ARP:0X806)
					����[46-1500]:��������;
					У��[4]:У��;


				etharp_hdr:
				+-----------------------------------------------------------------------------------------------------+
				+ hwhdr[2] | porotl[2] | hwlen[1] | porotllen[1] | op[2] | srcMAC[6] | srcIP[4] |desMAC[6] | desIP[4] +
				+-----------------------------------------------------------------------------------------------------+
				*ע: 
					hwhdr[2]:Ӳ�������ֶ�-��ʾӲ����ַ������,����ֵΪ1����ʾ��̫��MAC��ַ,����Ϊ6���ֽ�;
					porotl[2]:Э�������ֶα�ʾҪӳ���Э���ַ����,����ֵΪ0X800����ʾҪӳ���IP��ַ;
					hwlen[1]: Ӳ����ַ����,������̫��ARP�������Ӧ����˵,����ֵΪ6;
					porotllen[1]:Э���ַ����,������̫��ARP�������Ӧ����˵,����ֵΪ4;
					op[2]:�������� ARP����=1,ARPӦ��=2,RARP����=3,RARP=4;
					srcMAC[6]:���Ͷ�MAC��ַ;
					srcIP[4]: ���Ͷ�IP��ַ;
					desMAC[6]:Ŀ�Ķ�MAC��ַ
					desIP[4]: Ŀ�Ķ�IP��ַ;
				ip_hdr:
				+---------------------------------------------------------------------------------------------------------------------------------------------------------------------+
				+ v(bit4) | hdrlen(bit4) | TOS[1] | len[2] | flag0[2] | flag1(bit3) | offset(bit13) | ttl[1] | porotl[1] |  hdrchecsum[2] | srcIP[4] | desIP[4] | ѡ��[...] | data[N] +																								  +
				+---------------------------------------------------------------------------------------------------------------------------------------------------------------------+
				*ע: 
				v(bit4): �汾��,Ŀǰ�İ汾����4��IPv4;
				hdrlen(bit4): �ײ�����-ָ�ײ�վ32Bit�ֵ���Ŀ,������ѡ��,�������Ǹ�4bit����ײ��60�ֽ�;
				TOS[1]:����һ��3bit�������ֶ�,4bit TOS�ֶκ�1 bitδʹ�õ�����Ϊ0,�������4bit��Ϊ0,��һ�����;
				len[2]:����IP���ݱ��ĳ������ֽ�Ϊ��λ;
				flag0[2]:ָΨһ��ʾ�������͵�ÿһ�����ݱ�,ͨ��ÿ����һ�ݱ���ֵ�ͻ��1;
				flag1(bit3): ������IP���ݰ���Ƭʱʹ��;
				offset(bit13):  ������IP���ݰ���Ƭʱʹ��;
				ttl[1]:������IP���ݰ�����ܱ�ת�����ٴ�,ÿ����һ��ת��,��ֵ�ͻ��1,����ֵΪ0ʱ,һ��ICMP���Ļᱻ������Դ����;
				porotl[1]: Э���ֶ�������IP���ݰ��������ϲ��ĸ�Э��,��ֵΪ1��ʾΪICMP,2=IGMP,6=TCP,17=UDP;
				hdrchecsum[2]:�ײ�У���ֻ���IP�ײ���У��,�������������ڲ������ڴ�������г������,��������У�����ϲ�Э�鸺���;
				srcIP[4]:ԴIP��ַ;
				desIP[4]:Ŀ��IP��ַ;
				ѡ��[...]:ѡ��,���п���;
				data[N]:����data;
				
				icmp_hdr:
				+--------------------------------------------------------------------------------+
				+ type[1] | code[1] | checksum[2] | 	others[N](��ͬ�����ͺʹ����в�ͬ������)  |
				+--------------------------------------------------------------------------------+
				
				
**��ڲ���:
**���ز���:
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
**��������:	 	ipcEventProcFlylyEthernet
**��������:	 	
**��ڲ���:
**���ز���:
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
**��������:	 	ipcEventProcFlylyEthernet
**��������:	 	
**��ڲ���:
**���ز���:
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
**��������:	 	FlyEthernetTask
**��������:	 	
**��ڲ���:
**���ز���:
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
**��������:	 	FlyEthernetCreate
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
void FlyEthernetCreate(void)
{
	INT8U Res;
	Res = OSTaskCreate(FlyEthernetTask, 								//ִ�к���
				 NULL,	  												//����Ĳ���
				 &GstkFlyEthernet[FLYETHERNET_TASK_START_STK_SIZE-1],	//��ջ�ɸߵ�ַ���׵�ַ����
				 (INT8U)PRIO_FLYEHTERNET								//�������ȼ�
				 );
	if(OS_NO_ERR != Res)
	{
		LIBMCU_DEBUG(ETHERNTE_DEBUG,("\r\n Res = %d",Res));
	}
}
/***************************************************************************************************************************
**��������:	 	OpenMMCFile
**��������:	 	
**��ڲ���:
**���ز���:
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
**��������:	 	HttpsServicer
**��������:	 	
**��ڲ���:
**���ز���:
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
**��������:	 	fptWriteData
**��������:	 	
**��ڲ���:
**���ز���:
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
**��������:	 	fptServicer
**��������:	 	
**��ڲ���:
**���ز���:
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
**��������:	 	LwipTaskCreate
**��������:	 	
**��ڲ���:
**���ز���:
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
**��������:	 	LwipTaskCreate
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
void LwipTaskCreate(void)
{
	INT8U Res;
	Res = OSTaskCreate(LwipTask, 										//ִ�к���
				 NULL,	  												//����Ĳ���
				 &GstkLwip[LWIP_TASK_START_STK_SIZE-1],					//��ջ�ɸߵ�ַ���׵�ַ����
				 (INT8U)PRIO_LWIP										//�������ȼ�
				 );
	if(OS_NO_ERR != Res)
	{
		LIBMCU_DEBUG(ETHERNTE_DEBUG,("\r\n Res = %d",Res));
	}	
}
	
