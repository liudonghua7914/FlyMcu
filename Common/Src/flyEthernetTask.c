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


#if(APP_FTP == APP_WHAT)
/***************************************************************************************************************************
**��������:	 	ftp_recv_fn
**��������:	 	
**��ڲ���:
**���ز���:
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
**��������:	 	ftp_accept_fn
**��������:	 	
**��ڲ���:
**���ز���:
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
**��������:	 	ftp_sent_fn
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
err_t ftp_sent_fn(void *arg, struct tcp_pcb *tpcb,u16_t len)
{
	return ERR_OK;
}

/***************************************************************************************************************************
**��������:	 	fpt_new
**��������:	 	
**��ڲ���:
**���ز���:
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
**��������:	 	fpt_bind
**��������:	 	
**��ڲ���:
**���ز���:
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
**��������:	 	fpt_listen
**��������:	 	
**��ڲ���:
**���ز���:
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
**��������:	 	fpt_accpet
**��������:	 	
**��ڲ���:
**���ز���:
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
**��������:	 	fpt_connect
**��������:	 	
**��ڲ���:
**���ز���:
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
**��������:	 	ftp_arg
**��������:	 	
**��ڲ���:
**���ز���:
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
**��������:	 	ftp_poll
**��������:	 	
**��ڲ���:
**���ز���:
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
**��������:	 	ftp_recv
**��������:	 	
**��ڲ���:
**���ز���:
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
**��������:	 	ftp_sent
**��������:	 	
**��ڲ���:
**���ز���:
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
**��������:	 	fptWriteData
**��������:	 	
**��ڲ���:
**���ز���:
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
**��������:	 	OpenMMCFile
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
BOOL OpenMMCFile(char *name)
{	
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

}
/***************************************************************************************************************************
**��������:	 	fptServicer
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
void ftpServicer(void)
{
	
}
/***************************************************************************************************************************
**��������:	 	lwipServiceInit
**��������:	 	
**��ڲ���:
**���ز���:
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
	
