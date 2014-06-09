#define		_FLYETHERNETGOABLE_

#include "config.h"
#include "lpc17xx_lib.h"
#include "includes.h"
#include "UserType.h"


/***************************************************************************************************************************
**函数名称:	 	ipcFlyEthernetInit
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void ipcFlyEthernetInit(void)
{
	FlylyEthernetCreate();
	printf("\r\n ipcFlyEthernetInit OK");
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
**函数名称:	 	ReadEthernetData
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void ReadEthernetData(void)
{
	UINT len;
	BOOL bRes = FALSE;
	BYTE *rx_buffer;
	P_PACKTE_INFO eth_hrd;
	T_ARP_PACKET_INFO *arp_hrd;
	EMAC_PACKETBUF_Type PACKETBUF_Type;
	bRes = EMAC_CheckReceiveIndex();
	if (bRes) 
	{
		len = (RX_STAT_INFO(LPC_EMAC->RxConsumeIndex) & EMAC_RINFO_SIZE) - 1;
		printf("\r\n Ethernet len = %d ",len);
		if(len > RECSIZE)
		{	
			len = RECSIZE;
		}
		PACKETBUF_Type.pbDataBuf = (uint32_t *)flyEhternetInfo.recBuf;
		PACKETBUF_Type.ulDataLen = len;
		
		rx_buffer = (unsigned char *) RX_DESC_PACKET(LPC_EMAC->RxConsumeIndex);
		memcpy(flyEhternetInfo.recBuf,rx_buffer, len);
		
		eth_hrd = (P_PACKTE_INFO )flyEhternetInfo.recBuf;
		printf("\r\n+---------------START----------------+");
		printf("\r\n des %x:%x:%x:%x:%x:%x",eth_hrd->DesAddr[0],eth_hrd->DesAddr[1],eth_hrd->DesAddr[2],eth_hrd->DesAddr[3],eth_hrd->DesAddr[4],eth_hrd->DesAddr[5]);
		printf("\r\n src %x:%x:%x:%x:%x:%x",eth_hrd->SrcAddr[0],eth_hrd->SrcAddr[1],eth_hrd->SrcAddr[2],eth_hrd->SrcAddr[3],eth_hrd->SrcAddr[4],eth_hrd->SrcAddr[5]);
		printf("\r\n tpye: %x ",PP_HTONS(eth_hrd->Type));
		
		arp_hrd = (T_ARP_PACKET_INFO *)(&flyEhternetInfo.recBuf[14]);
		
		printf("\r\n hardware type: %d ",PP_HTONS(arp_hrd->HardType));//注意大小端问题
		printf("\r\n ProtocolType type: %04x ",PP_HTONS(arp_hrd->ProtocolType));
		printf("\r\n HardLen: %d ",arp_hrd->HardLen);
		printf("\r\n protocolLen: %d ",arp_hrd->protocolLen);
		printf("\r\n Op: %d ",PP_HTONS(arp_hrd->Op));
		printf("\r\n SrcMAC  %x:%x:%x:%x:%x:%x ",arp_hrd->SrcMAC[0],arp_hrd->SrcMAC[1],arp_hrd->SrcMAC[2],arp_hrd->SrcMAC[3],arp_hrd->SrcMAC[4],arp_hrd->SrcMAC[5]);
		printf("\r\n SrcIP   %d.%d.%d.%d ",arp_hrd->SrcIP[0],arp_hrd->SrcIP[1],arp_hrd->SrcIP[2],arp_hrd->SrcIP[3]);
		printf("\r\n DscMAC  %x:%x:%x:%x:%x:%x ",arp_hrd->SrcMAC[0],arp_hrd->DscMAC[1],arp_hrd->DscMAC[2],arp_hrd->DscMAC[3],arp_hrd->DscMAC[4],arp_hrd->DscMAC[5]);
		printf("\r\n DscIP   %d.%d.%d.%d ",arp_hrd->DscIP[0],arp_hrd->DscIP[1],arp_hrd->DscIP[2],arp_hrd->DscIP[3]);
		printf("\r\n+---------------END----------------+");
		EMAC_UpdateRxConsumeIndex();
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
	INT8U Error;
	while(1)
	{
		ReadEthernetData();
		OSTimeDly(OS_TICKS_PER_SEC/10);  	
	}
}
/***************************************************************************************************************************
**函数名称:	 	FlylyEthernetCreate
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void FlylyEthernetCreate(void)
{
	INT8U Res;
	Res = OSTaskCreate(FlyEthernetTask, 								//执行函数
				 NULL,	  											//带入的参数
				 &GstkFlyEthernet[FLYETHERNET_TASK_START_STK_SIZE-1],	//堆栈由高地址往底地址增长
				 (INT8U)PRIO_FLYEHTERNET								//任务优先级
				 );
	if(OS_NO_ERR != Res)
	{
		printf("\r\n Res = %d",Res);
	}
}
