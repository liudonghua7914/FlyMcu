#define		_FLYETHERNETGOABLE_

#include "config.h"
#include "lpc17xx_lib.h"
#include "includes.h"
#include "UserType.h"

BYTE ip_addr[] = {192,168,8,100};
BYTE mac_addr[] = {MAC0,MAC1,MAC2,MAC3,MAC4,MAC5};
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
**函数名称:	 	ip_input
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void ip_input(BYTE *p,UINT len)
{

}
/***************************************************************************************************************************
**函数名称:	 	arp_input
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void arp_input(BYTE *p,UINT len)
{
	BOOL bforus = FALSE;
	T_ARP_PACKET_INFO *arp_hrd;	
	P_PACKTE_INFO eth_hrd;
	
	eth_hrd = (P_PACKTE_INFO )(p);
	arp_hrd = (T_ARP_PACKET_INFO *)(p + 14);
	
	printf("\r\n hardware type: %d ",PP_HTONS(arp_hrd->HardType));//注意大小端问题
	printf("\r\n ProtocolType type: %04x ",PP_HTONS(arp_hrd->ProtocolType));
	printf("\r\n HardLen: %d ",arp_hrd->HardLen);
	printf("\r\n protocolLen: %d ",arp_hrd->protocolLen);
	printf("\r\n Op: %d ",PP_HTONS(arp_hrd->Op));
	printf("\r\n SrcMAC  %x:%x:%x:%x:%x:%x ",arp_hrd->SrcMAC[0],arp_hrd->SrcMAC[1],arp_hrd->SrcMAC[2],arp_hrd->SrcMAC[3],arp_hrd->SrcMAC[4],arp_hrd->SrcMAC[5]);
	printf("\r\n SrcIP   %d.%d.%d.%d ",arp_hrd->SrcIP[0],arp_hrd->SrcIP[1],arp_hrd->SrcIP[2],arp_hrd->SrcIP[3]);
	printf("\r\n DscMAC  %x:%x:%x:%x:%x:%x ",arp_hrd->SrcMAC[0],arp_hrd->DscMAC[1],arp_hrd->DscMAC[2],arp_hrd->DscMAC[3],arp_hrd->DscMAC[4],arp_hrd->DscMAC[5]);
	printf("\r\n DscIP   %d.%d.%d.%d ",arp_hrd->DscIP[0],arp_hrd->DscIP[1],arp_hrd->DscIP[2],arp_hrd->DscIP[3]);
	
	if((ip_addr[0] == arp_hrd->DscIP[0]) && 
	   (ip_addr[1] == arp_hrd->DscIP[1]) && 
	   (ip_addr[2] == arp_hrd->DscIP[2]) && 
	   (ip_addr[3] == arp_hrd->DscIP[3])
	  )
	{
		bforus = TRUE;
		printf("\r\n etharp_arp_input: replying to ARP request for our IP address....");
	}
	else
	{
		printf("\r\n etharp_arp_input: ARP request was not for us....");
	}
	
	if(bforus)
	{
		arp_hrd->Op = PP_HTONS(0X02);
		memcpy(arp_hrd->DscIP,arp_hrd->SrcIP,sizeof(arp_hrd->SrcIP));
		memcpy(arp_hrd->SrcIP,ip_addr,sizeof(ip_addr));
		
		memcpy(arp_hrd->DscMAC,arp_hrd->SrcMAC,sizeof(arp_hrd->SrcMAC));
		memcpy(arp_hrd->SrcMAC,mac_addr,sizeof(mac_addr));
		
		memcpy(eth_hrd->DesAddr,eth_hrd->SrcAddr,sizeof(eth_hrd->SrcAddr));
		memcpy(eth_hrd->SrcAddr,mac_addr,sizeof(mac_addr));
		
		WriteEthernetData(p,len);
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
		
		
		switch(PP_HTONS(eth_hrd->Type))
		{
			case 0X800:	//IP
						printf("\r\n IP Info ");
						ip_input(flyEhternetInfo.recBuf,len);
						break;
		
			case 0X806:	//ARP
						printf("\r\n ARP Info ");
						arp_input(flyEhternetInfo.recBuf,len);
						break;
			
			default:	printf("\r\n default Info ");
						break;
		}
			
		EMAC_UpdateRxConsumeIndex();
		printf("\r\n+---------------END----------------+");
	}
}
/***************************************************************************************************************************
**函数名称:	 	WriteEthernetData
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void WriteEthernetData(BYTE *p,UINT len)
{
	UINT32 IndexNext = LPC_EMAC->TxProduceIndex + 1;
	UINT32 Index;
	BYTE *tx_buffer;
	
	T_ARP_PACKET_INFO *arp_hrd;	
	P_PACKTE_INFO eth_hrd;
	
	
	
	if(len == 0)
	{
		return;
	}
	if(IndexNext == LPC_EMAC->TxConsumeIndex)
	{
		return;
	}
	
	Index = LPC_EMAC->TxProduceIndex;
	
	if (len > EMAC_ETH_MAX_FLEN)
	{
		len = EMAC_ETH_MAX_FLEN;
	}
	
	eth_hrd = (P_PACKTE_INFO )(p);
	arp_hrd = (T_ARP_PACKET_INFO *)(p + 14);
	printf("\r\n WriteEthernet: ");
	printf("srcIP: %d.%d.%d.%d ",arp_hrd->SrcIP[0],arp_hrd->SrcIP[1],arp_hrd->SrcIP[2],arp_hrd->SrcIP[3]);
	printf("dscIP: %d.%d.%d.%d ",arp_hrd->DscIP[0],arp_hrd->DscIP[1],arp_hrd->DscIP[2],arp_hrd->DscIP[3]);
	#if 0
	printf("srcMAC: %x:%x:%x:%x:%x:%x ",eth_hrd->SrcAddr[0],eth_hrd->SrcAddr[1],eth_hrd->SrcAddr[2],eth_hrd->SrcAddr[3],eth_hrd->SrcAddr[4],eth_hrd->SrcAddr[5]);
	printf("dscMAC: %x:%x:%x:%x:%x:%x ",eth_hrd->DesAddr[0],eth_hrd->DesAddr[1],eth_hrd->DesAddr[2],eth_hrd->DesAddr[3],eth_hrd->DesAddr[4],eth_hrd->DesAddr[5]);
	#else
	printf("srcMAC: %x:%x:%x:%x:%x:%x ",arp_hrd->SrcMAC[0],arp_hrd->SrcMAC[1],arp_hrd->SrcMAC[2],arp_hrd->SrcMAC[3],arp_hrd->SrcMAC[4],arp_hrd->SrcMAC[5]);
	printf("dscMAC: %x:%x:%x:%x:%x:%x ",arp_hrd->DscMAC[0],arp_hrd->DscMAC[1],arp_hrd->DscMAC[2],arp_hrd->DscMAC[3],arp_hrd->DscMAC[4],arp_hrd->DscMAC[5]);
	#endif
	
	tx_buffer = (unsigned char *)TX_DESC_PACKET(LPC_EMAC->TxProduceIndex);
	memcpy(tx_buffer,p,len);
	TX_DESC_CTRL(Index) &= ~0x7ff;
	TX_DESC_CTRL(Index) |= ((len - 1) & 0x7ff) | (EMAC_TCTRL_INT | EMAC_TCTRL_LAST); 
	EMAC_UpdateTxProduceIndex();
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
