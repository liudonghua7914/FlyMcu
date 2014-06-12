#define		_FLYETHERNETGOABLE_

#include "config.h"
#include "lpc17xx_lib.h"
#include "includes.h"
#include "UserType.h"


#define IP_PROTO_ICMP    1
#define IP_PROTO_IGMP    2
#define IP_PROTO_UDP     17
#define IP_PROTO_UDPLITE 136
#define IP_PROTO_TCP     6

BYTE ip_addr[] = {192,168,8,100};
BYTE gw_addr[] = {192, 168, 1, 1};
BYTE netmask[] = { 255, 255, 0, 0};	
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
	flyEhternetInfo.pbuf->payload = (void *)flyEhternetInfo.recBuf;
	memcpy(flyEhternetInfo.mynetif.ip_addr.addr,ip_addr,4);
	memcpy(flyEhternetInfo.mynetif.gw.addr,gw_addr,4);
	memcpy(flyEhternetInfo.mynetif.netmask.addr,netmask,4);
	memcpy(flyEhternetInfo.mynetif.hwaddr,mac_addr,sizeof(mac_addr));
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
**函数名称:	 	icmp_input
**函数功能:	 	icmp_input属于IP层
**入口参数:
**返回参数:
***************************************************************************************************************************/
void icmp_input(BYTE *p,UINT len)
{
	T_IP_HREAD_INFO *ip_hrd;
	T_ICMP_INFO *icmp_echo_hrd;
	BYTE offlen = 0;
	BYTE *q;
	BYTE type;
	ip_hrd  = (T_IP_HREAD_INFO *)(p + 14);
	offlen = (ip_hrd->v_hl & 0X0F) * 4;
	q = (BYTE *)(p + 14 + offlen);
	type = q[0];
	printf("\r\n icmp_input type: %x ",type);
	switch(type)
	{
		case ICMP_ER:	
						break;
		case ICMP_DUR:	
						break;
		case ICMP_SQ:
						break;
		case ICMP_RD:
						break;
		case ICMP_ECHO:	//请求回显ping
						icmp_echo_hrd = (T_ICMP_INFO *)(q);
						printf("\r\n icmp_input: ping...");
						memcpy(ip_hrd->dscIP,ip_hrd->srcIP,sizeof(ip_hrd->srcIP));
						memcpy(ip_hrd->srcIP,ip_addr,sizeof(ip_addr));
						icmp_echo_hrd->type = ICMP_ER;
						if(icmp_echo_hrd->chksum >= PP_HTONS(0xffffU - (ICMP_ECHO << 8))) 
						{
							icmp_echo_hrd->chksum += PP_HTONS(ICMP_ECHO << 8) + 1;
						}
						else 
						{
							icmp_echo_hrd->chksum += PP_HTONS(ICMP_ECHO << 8);
						}
			
						ip_hrd->ttl = 255;
						ip_hrd->chksum = 0;
						
						break;
		case ICMP_TE:
						break;
		case ICMP_TSR:
						break;
		case ICMP_IRQ:
						break;
		case ICMP_IR:
						break;
		default:
						break;
		
	}
}

/***************************************************************************************************************************
**函数名称:	 	pbuf_header
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
uint8_t pbuf_header(struct pbuf *pbuf,int offset)
{
	
}
/***************************************************************************************************************************
**函数名称:	 	ip_input
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void ip_input(struct pbuf *pbuf)
{
	BYTE Ver;
	BOOL bRes = FALSE;
	UINT i;
	T_PACKET_INFO *eth_hrd;
	T_IP_HREAD_INFO *ip_hrd;

	eth_hrd = (T_PACKET_INFO *)(pbuf->payload);
	ip_hrd  = (T_IP_HREAD_INFO *)(pbuf->payload + 14);
	
	Ver = PP_HTONS(ip_hrd->v_hl);
	printf("\r\n ip_input: ");
	printf("\r\n ver: %d ",(ip_hrd->v_hl >> 4) & 0x0f);
	printf("\r\n hdlen: %d ",(ip_hrd->v_hl >> 0) & 0x0f);
	printf("\r\n tos: %02x ",ip_hrd->tos);
	printf("\r\n len: %d ",PP_HTONS(ip_hrd->len));
	printf("\r\n id: %04x ",PP_HTONS(ip_hrd->id));
	
	printf("\r\n flag: %x ",PP_HTONS(ip_hrd->offset) >> 13);
	
	printf("\r\n offset: %d ",PP_HTONS(ip_hrd->offset) & 0X1FFF);
	printf("\r\n ttl: %d ",ip_hrd->ttl);
	printf("\r\n proto: %d ",ip_hrd->proto);
	printf("\r\n chksum: %04x ",PP_HTONS(ip_hrd->chksum));
	printf("\r\n srcIP   %d.%d.%d.%d ",ip_hrd->srcIP[0],ip_hrd->srcIP[1],ip_hrd->srcIP[2],ip_hrd->srcIP[3]);
	printf("\r\n dscIP   %d.%d.%d.%d ",ip_hrd->dscIP[0],ip_hrd->dscIP[1],ip_hrd->dscIP[2],ip_hrd->dscIP[3]);
	
	
	if((ip_addr[0] == ip_hrd->dscIP[0]) && 
	   (ip_addr[1] == ip_hrd->dscIP[1]) && 
	   (ip_addr[2] == ip_hrd->dscIP[2]) && 
	   (ip_addr[3] == ip_hrd->dscIP[3])
	  )
	{
		bRes = TRUE;
	}
	else
	{
		printf("\r\n ip_input: packet not for us....");
	}
	
	
	if(bRes)
	{
		memcpy(flyEhternetInfo.currentDesIP,ip_hrd->dscIP,4);
		memcpy(flyEhternetInfo.currentSrcIP,ip_hrd->srcIP,4);
		memcpy(flyEhternetInfo.currentDesMac,eth_hrd->DesAddr,6);
		memcpy(flyEhternetInfo.currentDesMac,eth_hrd->SrcAddr,6);
		
		switch(ip_hrd->proto)
		{
			case IP_PROTO_TCP: 	printf("\r\n IP_PROTO_TCP");
								break;
			
			case IP_PROTO_UDP:	printf("\r\n IP_PROTO_UDP");
								break;
			
			case IP_PROTO_ICMP:	printf("\r\n IP_PROTO_ICMP");
								icmp_input(p,len);
								break;
			
			case IP_PROTO_UDPLITE:
								printf("\r\n IP_PROTO_UDPLITE");
								break;
			
			default:			printf("\r\n proto default...");	
								break;
		}
	}
}
/***************************************************************************************************************************
**函数名称:	 	etharp_arp_input
**函数功能:	 	
				eth_hdr:
				+----------------------------------------------------------------------------------------+
				+  desMAC[6]  |  srcMAC[6]  |  type[2]( ARP / IP)  |  	数据[46-1500]   |	校验[4]      +
				+----------------------------------------------------------------------------------------+
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
**入口参数:
**返回参数:
***************************************************************************************************************************/
void etharp_arp_input(struct pbuf *pbuf, struct eth_addr *ethaddr,struct netif *netif)
{
	BOOL bforus = FALSE;
	struct etharp_hdr *arp_hrd;	
	struct eth_hdr* ethhdr;
	
	ethhdr = (struct eth_hdr* )(pbuf->payload);
	arp_hrd = (struct etharp_hdr *)((BYTE *)(pbuf->payload + 14));
	
	
	
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
**函数名称:	 	ethernet_input
**函数功能:		
				eth_hdr:
				+----------------------------------------------------------------------------------------+
				+  desMAC[6]  |  srcMAC[6]  |  type[2]( ARP / IP)  |  	数据[46-1500]   |	校验[4]      +
				+----------------------------------------------------------------------------------------+
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
				
**入口参数:
**返回参数:
***************************************************************************************************************************/
int ethernet_input(struct pbuf *pbuf,struct netif *netif)
{
	struct eth_hdr* ethhdr;
	if(pbuf->len < SIZEOF_ETH_HDR)
	{
		printf("\r\n packet not for us");
		return ERR;
	}
	
	ethhdr = (struct eth_hdr*)pbuf->payload;
	printf("\r\n srcMAC: %x:%x:%x:%x:%x:%x",ethhdr->SrcAddr[0],ethhdr->SrcAddr[1],ethhdr->SrcAddr[2],ethhdr->SrcAddr[3],ethhdr->SrcAddr[4],ethhdr->SrcAddr[5]);
	printf("\r\n desMAC: %x:%x:%x:%x:%x:%x",ethhdr->DesAddr[0],ethhdr->DesAddr[1],ethhdr->DesAddr[2],ethhdr->DesAddr[3],ethhdr->DesAddr[4],ethhdr->DesAddr[5]);
	
	switch(ethhdr->Type)
	{
		case PP_HTONS(ETHTYPE_ARP): printf("\r\n ARP Info ");
									etharp_arp_input(pbuf,(struct eth_addr *)netif->hwaddr,netif);
									break;
		case PP_HTONS(ETHTYPE_IP):	printf("\r\n IP Info ");	
									break;
		default:
									break;
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
	struct eth_hdr* ethhdr;
	
	bRes = EMAC_CheckReceiveIndex();
	if (bRes) 
	{
		len = (RX_STAT_INFO(LPC_EMAC->RxConsumeIndex) & EMAC_RINFO_SIZE) - 1;
		printf("\r\n Ethernet len = %d ",len);
		if(len > RECSIZE)
		{	
			len = RECSIZE;
		}
	
		
		rx_buffer = (unsigned char *) RX_DESC_PACKET(LPC_EMAC->RxConsumeIndex);
		memcpy((unsigned char *)flyEhternetInfo.pbuf->payload,rx_buffer, len);
		flyEhternetInfo.pbuf->len = len;
	
		ethhdr = (struct eth_hdr *)flyEhternetInfo.pbuf->payload;
		printf("\r\n+---------------START----------------+");
		
		
		
		switch(PP_HTONS(ethhdr->Type))
		{
			case 0X800:	//IP
					
			case 0X806:	//ARP				
						ethernet_input(flyEhternetInfo.pbuf,&flyEhternetInfo.mynetif);
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
	
	T_ARP_HEAD_INFO *arp_hrd;	
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
	arp_hrd = (T_ARP_HEAD_INFO *)(p + 14);
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
