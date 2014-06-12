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
void icmp_input(struct pbuf *pbuf,struct netif *inp)
{
#if 0
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
#endif
}

/***************************************************************************************************************************
**函数名称:	 	pbuf_header
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
uint8_t pbuf_header(struct pbuf *pbuf,int offset)
{
	BYTE type;
	BYTE *playload;
	UINT len = sizeof(struct pbuf);
	uint16_t increment_magnitude;
	printf("\r\n pbuf_header: len %d offset %d ",len,offset);
	if ((offset == 0) || (pbuf == NULL)) 
	{
		return 0;
	}
	if (offset < 0)
	{
		increment_magnitude = -offset;
	} 
	else 
	{
		increment_magnitude = offset;
	}
	
	type = pbuf->type;
	playload = pbuf->payload;
	if (type == PBUF_RAM || type == PBUF_POOL) 
	{
		pbuf->payload = (BYTE *)pbuf->payload - offset; 
		if((BYTE *)pbuf->payload < (BYTE *)flyEhternetInfo.playload)//防止pbuf->payload指错
		{
			printf("\r\n pbuf_header: failed  (not enough space for new header size)");
			pbuf->payload = playload;
			return 1;
		}
	}
	else
	{
		return 1;
	}
	
	pbuf->len += offset;
	pbuf->tot_len += offset;
	printf("\r\n pbuf_header: len %d tot_len %d ",pbuf->len,pbuf->tot_len);
	return 0;
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
**入口参数:
**返回参数:
***************************************************************************************************************************/
uint8_t ip_input(struct pbuf *pbuf,struct netif *inp)
{
	
	struct ip_hdr *iphdr;
	struct netif *netif;
	UINT16 iphdr_hlen;
	UINT16 iphdr_len;
	UINT16 v_hl_tos;
	BOOL bRes = FALSE;
	
	iphdr = (struct ip_hdr *)pbuf->payload;
	printf("\r\n ip_input:");
	printf("\r\n v_hl_tos = %04x",PP_HTONS(iphdr->v_hl_tos));
	printf("\r\n len = %d ",PP_HTONS(iphdr->len));
	printf("\r\n id = %04x ",PP_HTONS(iphdr->id));
	printf("\r\n offset = %d ",PP_HTONS(iphdr->offset));
	printf("\r\n ttl: %d ",iphdr->ttl);
	printf("\r\n proto: %d ",iphdr->proto);
	printf("\r\n chksum: %04x ",PP_HTONS(iphdr->chksum));
	printf("\r\n srcIP   %d.%d.%d.%d ",iphdr->srcIP[0],iphdr->srcIP[1],iphdr->srcIP[2],iphdr->srcIP[3]);
	printf("\r\n dscIP   %d.%d.%d.%d ",iphdr->dscIP[0],iphdr->dscIP[1],iphdr->dscIP[2],iphdr->dscIP[3]);
	
	v_hl_tos = PP_HTONS(iphdr->v_hl_tos);
	if((v_hl_tos >> 12) !=  4)
	{
		printf("\r\n IP packet dropped due to bad version number");
		return 0;
	}
	
	iphdr_hlen =(v_hl_tos >> 8) & 0X0F;
	iphdr_hlen *= 4;
	printf("\r\n iphdr_hlen %d ",iphdr_hlen);
	iphdr_len = PP_HTONS(iphdr->len);
	
	if((iphdr_hlen > pbuf->len) || (iphdr_len > pbuf->tot_len))
	{
		if(iphdr_hlen > pbuf->len) 
		{
			printf("\r\n IP header (len % d) does not fit in first pbuf (len %d), IP packet dropped",iphdr_len,pbuf->len);
		}
		
		if(iphdr_len > pbuf->tot_len) 
		{
			printf("\r\n IP (len %d) is longer than pbuf (len %d), IP packet dropped",iphdr_len,pbuf->tot_len);
		}
		return ERR_OK;
	}
	
	//checksum......暂时忽略...
	
	if((ip_addr[0] == iphdr->dscIP[0]) && 
	   (ip_addr[1] == iphdr->dscIP[1]) && 
	   (ip_addr[2] == iphdr->dscIP[2]) && 
	   (ip_addr[3] == iphdr->dscIP[3])
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
		switch(iphdr->proto)
		{
			case IP_PROTO_TCP: 	printf("\r\n IP_PROTO_TCP");
								break;
			
			case IP_PROTO_UDP:	printf("\r\n IP_PROTO_UDP");
								break;
			
			case IP_PROTO_ICMP:	printf("\r\n IP_PROTO_ICMP");
								//icmp_input(p,len);
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
				
				
**入口参数:
**返回参数:
***************************************************************************************************************************/
void etharp_arp_input(struct pbuf *pbuf, struct eth_addr *ethaddr,struct netif *netif)
{
	BOOL bforus = FALSE;
	struct etharp_hdr *arp_hrd;	
	struct eth_hdr *ethhdr;
	BYTE *p;
	ethhdr = (struct eth_hdr* )(pbuf->payload);
	arp_hrd = (struct etharp_hdr *)((BYTE *)pbuf->payload + 14);
	
	
	
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
		
		memcpy(ethhdr->DesAddr,ethhdr->SrcAddr,sizeof(ethhdr->SrcAddr));
		memcpy(ethhdr->SrcAddr,mac_addr,sizeof(mac_addr));
		p = (BYTE *)pbuf->payload;
		WriteEthernetData(p,pbuf->len);
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
**入口参数:
**返回参数:
***************************************************************************************************************************/
int ethernet_input(struct pbuf *pbuf,struct netif *netif)
{
	struct eth_hdr* ethhdr;
	int ip_hdr_offset = SIZEOF_ETH_HDR;
	if(pbuf->len < SIZEOF_ETH_HDR)
	{
		printf("\r\n packet not for us");
		return ERR;
	}
	
	ethhdr = (struct eth_hdr*)pbuf->payload;
	flyEhternetInfo.playload = pbuf->payload;
	
	printf("\r\n srcMAC: %x:%x:%x:%x:%x:%x",ethhdr->SrcAddr[0],ethhdr->SrcAddr[1],ethhdr->SrcAddr[2],ethhdr->SrcAddr[3],ethhdr->SrcAddr[4],ethhdr->SrcAddr[5]);
	printf("\r\n desMAC: %x:%x:%x:%x:%x:%x",ethhdr->DesAddr[0],ethhdr->DesAddr[1],ethhdr->DesAddr[2],ethhdr->DesAddr[3],ethhdr->DesAddr[4],ethhdr->DesAddr[5]);
	
	switch(ethhdr->Type)
	{
		case PP_HTONS(ETHTYPE_ARP): printf("\r\n ARP Info ");
									etharp_arp_input(pbuf,(struct eth_addr *)netif->hwaddr,netif);
									break;
		case PP_HTONS(ETHTYPE_IP):	printf("\r\n IP Info ");
									if(pbuf_header(pbuf, -ip_hdr_offset)) 
									{
										printf("\r\n pbuf_header fail");
									}
									else
									{
										ip_input(pbuf,netif);	
									}
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
	struct pbuf *p;
	p = &flyEhternetInfo.pbuf;
	p->payload = (BYTE *)flyEhternetInfo.recBuf;
	bRes = EMAC_CheckReceiveIndex();
	if (bRes) 
	{
		len = (RX_STAT_INFO(LPC_EMAC->RxConsumeIndex) & EMAC_RINFO_SIZE) - 1;
		if(len > RECSIZE)
		{	
			len = RECSIZE;
		}
		
		rx_buffer = (unsigned char *) RX_DESC_PACKET(LPC_EMAC->RxConsumeIndex);
		memcpy(p->payload,rx_buffer, len);
		p->len = len;
		p->tot_len = len;
		ethhdr = (struct eth_hdr *)p->payload;
		printf("\r\n+---------------START----------------+");
		
	
		switch(PP_HTONS(ethhdr->Type))
		{
			case 0X800:	//IP
					
			case 0X806:	//ARP				
						ethernet_input(p,&flyEhternetInfo.mynetif);
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
	
	struct etharp_hdr *arp_hrd;	
	struct eth_hdr* ethhdr;
	
	
	
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
	
	ethhdr = (struct eth_hdr *)(p);
	arp_hrd = (struct etharp_hdr *)(p + 14);
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
