#define		_FLYETHERNETGOABLE_
#if 0
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
	memcpy(flyEhternetInfo.mynetif.hwaddr.addr,mac_addr,sizeof(mac_addr));
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
	
	printf("\r\n pbuf_header type: %d ",pbuf->type);
	
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
	printf("\r\n pbuf_header type2: %d ",type);
	playload = pbuf->payload;
	if (type == PBUF_RAM || type == PBUF_POOL) 
	{
		pbuf->payload = (BYTE *)pbuf->payload - offset; 
		if((BYTE *)pbuf->payload < (BYTE *)flyEhternetInfo.playload)//防止pbuf->payload指错
		{
			printf("\r\n pbuf_header: failed  (not enough space for new header size) %p Min(%p)",(void *)pbuf->payload,(void *)flyEhternetInfo.playload);
			pbuf->payload = playload;
			
			return 1;
		}
	}
	else
	{
		printf("\r\n type is not PBUF_RAM or PBUF_POOL %d ",type);
		return 1;
	}
	
	pbuf->len += offset;
	pbuf->tot_len += offset;
	printf ("\r\n pbuf_header: old %p new %p (%hd)\n",(void *)playload, (void *)pbuf->payload, offset);
	return 0;
}
/***************************************************************************************************************************
**函数名称:	 	inet_chksum
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
UINT16 inet_chksum(void *dataptr, UINT16 len)
{
	
	BYTE *pb = (BYTE *)dataptr;
	UINT16 *ps, t = 0;
	UINT32 sum = 0;
	int odd = ((uintptr_t)pb & 1);
	
	printf("inet_chksum \n");		
	if (odd && len > 0) 
	{
		((BYTE *)&t)[1] = *pb++;
		len--;
	}

	/* Add the bulk of the data */
	ps = (UINT16 *)(void *)pb;
	while (len > 1) 
	{
		sum += *ps++;
		len -= 2;
	}

	/* Consume left-over byte, if any */
	if (len > 0) 
	{
		((BYTE *)&t)[0] = *(BYTE *)ps;
	}

	/* Add end bytes */
	sum += t;

	/* Fold 32-bit sum to 16 bits
	 calling this twice is propably faster than if statements... */
	sum = FOLD_U32T(sum);
	sum = FOLD_U32T(sum);

	/* Swap if alignment was odd */
	if (odd) 
	{
		sum = SWAP_BYTES_IN_WORD(sum);
	}
	return ~sum;
}
/***************************************************************************************************************************
**函数名称:	 	etharp_send_ip
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
static err_t etharp_send_ip(struct netif *netif, struct pbuf *p, struct eth_addr *src, struct eth_addr *dst)
{
	struct eth_hdr *ethhdr = (struct eth_hdr *)p->payload;
	struct ip_hdr *iphdr = (struct ip_hdr *)((BYTE *)p->payload + 14);
	
	printf("\r\n etharp_send_ip tpye:%d ",p->type);
	
	memcpy(ethhdr->DesMac.addr,dst->addr,6);
	memcpy(ethhdr->SrcMac.addr,src->addr,6);
	ethhdr->Type = PP_HTONS(ETHTYPE_IP);
	
	
	printf("\r\n src: %x:%x:%x:%x:%x:%x ",ethhdr->SrcMac.addr[0],ethhdr->SrcMac.addr[1],ethhdr->SrcMac.addr[2],ethhdr->SrcMac.addr[3],ethhdr->SrcMac.addr[4],ethhdr->SrcMac.addr[5]);
	printf("\r\n dst: %x:%x:%x:%x:%x:%x ",ethhdr->DesMac.addr[0],ethhdr->DesMac.addr[1],ethhdr->DesMac.addr[2],ethhdr->DesMac.addr[3],ethhdr->DesMac.addr[4],ethhdr->DesMac.addr[5]);
	printf("\r\n tpye :%04x ",ethhdr->Type);
	
	printf("\r\n src.addr %d.%d.%d.%d ",iphdr->srcIP.addr[0],iphdr->srcIP.addr[1],iphdr->srcIP.addr[2],iphdr->srcIP.addr[3]);
	printf("\r\n dest.addr %d.%d.%d.%d",iphdr->dscIP.addr[0],iphdr->dscIP.addr[1],iphdr->dscIP.addr[2],iphdr->dscIP.addr[3]);
	
	WriteEthernetData((BYTE *)p->payload,p->len);
}
/***************************************************************************************************************************
**函数名称:	 	etharp_output
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
err_t etharp_output(struct netif *netif, struct pbuf *q, ip_addr_t *ipaddr)
{
	struct eth_addr *dest, mcastaddr;
	printf("\r\n etharp_output tpye:%d ",q->type);

	if (pbuf_header(q, sizeof(struct eth_hdr)) != 0) 
	{
		printf("\r\n etharp_output: could not allocate room for header");
		return ERR_BUF;
	}
	dest = NULL;
	etharp_send_ip(netif,q,&netif->hwaddr,&flyEhternetInfo.currentSrcMac);
}
/***************************************************************************************************************************
**函数名称:	 	ip_output_if
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
err_t ip_output_if(struct pbuf *pbuf, ip_addr_t *src, ip_addr_t *dest,BYTE ttl, BYTE tos,BYTE proto, struct netif *netif)
{
	struct ip_hdr *iphdr;
	ip_addr_t dest_addr;
	printf("\r\n ip_output_if tpye:%d ",pbuf->type);
	if(dest != IP_HDRINCL)
	{
	
	}
	else
	{
		iphdr = (struct ip_hdr *)pbuf->payload;
		etharp_output(netif,pbuf,&iphdr->dscIP);
	}
	return ERR_OK;
}
/***************************************************************************************************************************
**函数名称:	 	icmp_input
**函数功能:	 	icmp_input属于IP层
				
				icmp_hdr:
				+--------------------------------------------------------------------------------+
				+ type[1] | code[1] | checksum[2] | 	others[N](不同的类型和代码有不同的内容)  |
				+--------------------------------------------------------------------------------+
**入口参数:
**返回参数:
***************************************************************************************************************************/
void icmp_input(struct pbuf *pbuf,struct netif *inp)
{
	BYTE type;

	struct icmp_echo_hdr *iecho;
	struct ip_hdr *iphdr;
	int hlen;
	UINT16 v_hl_tos;
	iphdr = (struct ip_hdr *)pbuf->payload;
	v_hl_tos = PP_HTONS(iphdr->v_hl_tos);
	hlen =(v_hl_tos >> 8) & 0X0F;
	hlen *= 4;
	
	printf("\r\n icmp_input: tpye:%d ",pbuf->type);
	
	if(pbuf_header(pbuf,-hlen) || (pbuf->tot_len < sizeof(UINT16) * 2))
	{
		printf("\r\n cmp_input: short ICMP (%d bytes) received ",pbuf->tot_len);
	}
	
	type = *(BYTE *)pbuf->payload;
	printf("\r\n type : %x ",type);
	
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
		case ICMP_ECHO:	
						printf("\r\n Ping....");
						if(pbuf->tot_len < sizeof(struct icmp_echo_hdr))
						{
							printf("\r\n icmp_input: bad ICMP echo received");
						}
						
						if(pbuf_header(pbuf,PBUF_IP_HLEN + SIZEOF_ETH_HDR))
						{
							printf("\r\n icmp_input: pbuf_header");
						}
						else
						{
							printf("\r\n icmp_input: else pbuf_header");
							if(pbuf_header(pbuf,-(PBUF_IP_HLEN + SIZEOF_ETH_HDR)))
							{
								printf("\r\n icmp_input: restoring original p->payload failed");
							}
						}
						
						iecho = (struct icmp_echo_hdr *)pbuf->payload;		
						
						memcpy(iphdr->dscIP.addr,iphdr->srcIP.addr,sizeof(iphdr->dscIP.addr));
						memcpy(iphdr->srcIP.addr,ip_addr,sizeof(ip_addr));
						iecho->type = ICMP_ER;
						if (iecho->chksum >= PP_HTONS(0xffffU - (ICMP_ECHO << 8))) 
						{
							iecho->chksum += PP_HTONS(ICMP_ECHO << 8) + 1;
						} 
						else 
						{
							iecho->chksum += PP_HTONS(ICMP_ECHO << 8);
						}
						iphdr->ttl = 255;
						iphdr->chksum = 0;
						iphdr->chksum = inet_chksum(iphdr,20);
						if(pbuf_header(pbuf, hlen)) 
						{
							printf("\r\n icmp_input pbuf_header error");
						}
						else
						{
							err_t ret;
							/* send an ICMP packet, src addr is the dest addr of the curren packet */
							ret = ip_output_if(pbuf,&flyEhternetInfo.currentDesIP, IP_HDRINCL,
										   ICMP_TTL, 0, IP_PROTO_ICMP, inp);
							if (ret != ERR_OK) 
							{
								printf("icmp_input: ip_output_if returned an error: %c.\n", ret);
							}
						}
						break;
		default:
						break;
		
	}
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
err_t ip_input(struct pbuf *pbuf,struct netif *inp)
{
	
	struct ip_hdr *iphdr;
	struct netif *netif;
	UINT16 iphdr_hlen;
	UINT16 iphdr_len;
	UINT16 v_hl_tos;
	BOOL bRes = FALSE;
	
	iphdr = (struct ip_hdr *)pbuf->payload;
	printf("\r\n ip_input: tpye:%d ",pbuf->type);
	printf("\r\n v_hl_tos = %04x",PP_HTONS(iphdr->v_hl_tos));
	printf("\r\n len = %d ",PP_HTONS(iphdr->len));
	printf("\r\n id = %04x ",PP_HTONS(iphdr->id));
	printf("\r\n offset = %d ",PP_HTONS(iphdr->offset));
	printf("\r\n ttl: %d ",iphdr->ttl);
	printf("\r\n proto: %d ",iphdr->proto);
	printf("\r\n chksum: %04x ",PP_HTONS(iphdr->chksum));
	printf("\r\n srcIP   %d.%d.%d.%d ",iphdr->srcIP.addr[0],iphdr->srcIP.addr[1],iphdr->srcIP.addr[2],iphdr->srcIP.addr[3]);
	printf("\r\n dscIP   %d.%d.%d.%d ",iphdr->dscIP.addr[0],iphdr->dscIP.addr[1],iphdr->dscIP.addr[2],iphdr->dscIP.addr[3]);
	
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
	if(inet_chksum(iphdr, iphdr_hlen) != 0)
	{
		printf ("Checksum (0x%hx) failed, IP packet dropped.\n", inet_chksum(iphdr, iphdr_hlen));
	}
	
	if((ip_addr[0] == iphdr->dscIP.addr[0]) && 
	   (ip_addr[1] == iphdr->dscIP.addr[1]) && 
	   (ip_addr[2] == iphdr->dscIP.addr[2]) && 
	   (ip_addr[3] == iphdr->dscIP.addr[3])
	  )
	{
		memcpy(flyEhternetInfo.currentSrcIP.addr,iphdr->srcIP.addr,4);
		memcpy(flyEhternetInfo.currentDesIP.addr,iphdr->dscIP.addr,4);
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
								icmp_input(pbuf,inp);
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
				
				icmp_hdr:
				+--------------------------------------------------------------------------------+
				+ type[1] | code[1] | checksum[2] | 	others[N](不同的类型和代码有不同的内容)  |
				+--------------------------------------------------------------------------------+
				
				udp_hdr:
				+-----------------------------------------------------------------+
				+ src[2] | dest[2] | len[2] | checksum[2] |  data[N](如果有)	  +					
				+-----------------------------------------------------------------+
				*注:
				src[2]: 	16位源端口号;
				dest[2]: 	16位目标端口号;
				len[2]: 	16位UDP长度;
				checksum[2]:16位检验和;
				data[N]:	数据(如果有);
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
		
		memcpy(ethhdr->DesMac.addr,ethhdr->SrcMac.addr,sizeof(ethhdr->SrcMac.addr));
		memcpy(ethhdr->SrcMac.addr,mac_addr,sizeof(mac_addr));
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
		return ERR_OK;
	}
	
	ethhdr = (struct eth_hdr*)pbuf->payload;
	flyEhternetInfo.playload = pbuf->payload;
	
	printf("\r\n srcMAC: %x:%x:%x:%x:%x:%x",ethhdr->SrcMac.addr[0],ethhdr->SrcMac.addr[1],ethhdr->SrcMac.addr[2],ethhdr->SrcMac.addr[3],ethhdr->SrcMac.addr[4],ethhdr->SrcMac.addr[5]);
	printf("\r\n desMAC: %x:%x:%x:%x:%x:%x",ethhdr->DesMac.addr[0],ethhdr->DesMac.addr[1],ethhdr->DesMac.addr[2],ethhdr->DesMac.addr[3],ethhdr->DesMac.addr[4],ethhdr->DesMac.addr[5]);
	
	
	memcpy(flyEhternetInfo.currentSrcMac.addr,ethhdr->SrcMac.addr,6);
	memcpy(flyEhternetInfo.currentDesMac.addr,ethhdr->DesMac.addr,6);
	
	
	
	switch(ethhdr->Type)
	{
		case PP_HTONS(ETHTYPE_ARP): printf("\r\n ARP Info ");
									etharp_arp_input(pbuf,&netif->hwaddr,netif);
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
	flyEhternetInfo.pbuf.type = PBUF_POOL;
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
	struct ip_hdr *iphdr;
	printf("\r\n WriteEthernetData: %d ",len);
	
	
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
	
	printf("\r\n WriteEthernet: ");
	
	ethhdr = (struct eth_hdr *)(p);	

	printf("\r\n src: %x:%x:%x:%x:%x:%x ",ethhdr->SrcMac.addr[0],ethhdr->SrcMac.addr[1],ethhdr->SrcMac.addr[2],ethhdr->SrcMac.addr[3],ethhdr->SrcMac.addr[4],ethhdr->SrcMac.addr[5]);
	printf("\r\n dst: %x:%x:%x:%x:%x:%x ",ethhdr->DesMac.addr[0],ethhdr->DesMac.addr[1],ethhdr->DesMac.addr[2],ethhdr->DesMac.addr[3],ethhdr->DesMac.addr[4],ethhdr->DesMac.addr[5]);
	
	if(ETHTYPE_IP == PP_HTONS(ethhdr->Type))
	{
		iphdr = (struct ip_hdr *)(p + 14);
		printf("\r\n v_hl_tos = %04x",PP_HTONS(iphdr->v_hl_tos));
		printf("\r\n len = %d ",PP_HTONS(iphdr->len));
		printf("\r\n id = %04x ",PP_HTONS(iphdr->id));
		printf("\r\n offset = %d ",PP_HTONS(iphdr->offset));
		printf("\r\n ttl: %d ",iphdr->ttl);
		printf("\r\n proto: %d ",iphdr->proto);
		printf("\r\n chksum: %04x ",PP_HTONS(iphdr->chksum));
		printf("\r\n srcIP   %d.%d.%d.%d ",iphdr->srcIP.addr[0],iphdr->srcIP.addr[1],iphdr->srcIP.addr[2],iphdr->srcIP.addr[3]);
		printf("\r\n dscIP   %d.%d.%d.%d ",iphdr->dscIP.addr[0],iphdr->dscIP.addr[1],iphdr->dscIP.addr[2],iphdr->dscIP.addr[3]);
	}
	else if(ETHTYPE_ARP == PP_HTONS(ethhdr->Type))
	{
		arp_hrd = (struct etharp_hdr *)(p + 14);
		
		printf("srcIP: %d.%d.%d.%d ",arp_hrd->SrcIP[0],arp_hrd->SrcIP[1],arp_hrd->SrcIP[2],arp_hrd->SrcIP[3]);
		printf("dscIP: %d.%d.%d.%d ",arp_hrd->DscIP[0],arp_hrd->DscIP[1],arp_hrd->DscIP[2],arp_hrd->DscIP[3]);

		printf("srcMAC: %x:%x:%x:%x:%x:%x ",arp_hrd->SrcMAC[0],arp_hrd->SrcMAC[1],arp_hrd->SrcMAC[2],arp_hrd->SrcMAC[3],arp_hrd->SrcMAC[4],arp_hrd->SrcMAC[5]);
		printf("dscMAC: %x:%x:%x:%x:%x:%x ",arp_hrd->DscMAC[0],arp_hrd->DscMAC[1],arp_hrd->DscMAC[2],arp_hrd->DscMAC[3],arp_hrd->DscMAC[4],arp_hrd->DscMAC[5]);

	}

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
#endif