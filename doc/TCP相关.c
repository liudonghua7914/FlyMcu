
pcb链表:
	1.处于侦听状态的链表 tcp_listen_pcbs -> 链表用来连接处于 LISTEN 状态的控制块;
	2.处于稳定状态的链表 tcp_active_pcbs -> 链表用来连接处于 TCP状态转换图中其他所有状态的控制块;
	3.已经绑定完毕的PCB链表 tcp_bound_pcbs -> 链表用来连接新创建的控制块，可以认为新建的控制块处于 closed状态;
	4.处于TIME-WAIT状态的PCB链表 tcp_tw_pcbs -> 链表用来连接处于TIME_WAIT状态的控制块;
	
	   如果遍历完这些链表后，都没有找到相应的<IP 地址、端口>对，则说明该<IP 地址、端口>
	对可用，则可进行上面说的赋值操作，最后，函数将这个 PCB 加入绑定完毕的 PCB 链表
	tcp_bound_pcbs。


tcp_input:

	1. 获取数据头信息
	iphdr = (struct ip_hdr *)p->payload;
	tcphdr = (struct tcp_hdr *)((u8_t *)p->payload + IPH_HL(iphdr) * 4);
	
	2.将p指针向后移动 IPH_HL(iphdr) * 4个字节位置
	 if (pbuf_header(p, -((s16_t)(IPH_HL(iphdr) * 4))) || (p->tot_len < sizeof(struct tcp_hdr)))
	 {
		xxx
	 }
	 
	3.将p指针指向TCP数据项,指针移动hdrlen * 4个字节位置
	 hdrlen = TCPH_HDRLEN(tcphdr);
	 if(pbuf_header(p, -(hdrlen * 4))
	 {
		xxx
	 }
	 
	4.大小端模式转换
	tcphdr->src = ntohs(tcphdr->src);
	tcphdr->dest = ntohs(tcphdr->dest);
	seqno = tcphdr->seqno = ntohl(tcphdr->seqno);
	ackno = tcphdr->ackno = ntohl(tcphdr->ackno);
	tcphdr->wnd = ntohs(tcphdr->wnd);

	flags = TCPH_FLAGS(tcphdr);
	tcplen = p->tot_len + ((flags & (TCP_FIN | TCP_SYN)) ? 1 : 0);
	
	5.遍历真个tcp_active_pcbs指向的链表
	//tcp_active_pcbs = 自己初始化时定义的一个tcp_pcb结构体
	for(pcb = tcp_active_pcbs; pcb != NULL; pcb = pcb->next) 
	{
		if (pcb->remote_port == tcphdr->src &&
		pcb->local_port == tcphdr->dest &&
		ip_addr_cmp(&(pcb->remote_ip), &current_iphdr_src) &&
		ip_addr_cmp(&(pcb->local_ip), &current_iphdr_dest)) 
		{
			//源端口号 = remote_port
			//目的端口号 = local_port
			//当前源 IP = pcb->remote_ip
			//当前目标 IP = pcb->local_ip
			
			if (prev != NULL) 
			{
				prev->next = pcb->next;
				pcb->next = tcp_active_pcbs;
				tcp_active_pcbs = pcb;
			}	
			else
			{
			
			}
		}
		prev = pcb;
	}
	
	
	
	
	
	/***************************************************************************************************************************
**oˉêy??3?:	 	ReadEthernetData
**oˉêy1|?ü:	 	
**è??ú2?êy:
**·μ??2?êy:
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
**oˉêy??3?:	 	WriteEthernetData
**oˉêy1|?ü:	 	
**è??ú2?êy:
**·μ??2?êy:
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






1. etharp_send_ip->netif->linkoutput(netif, p);
2. etharp_arp_input->netif->linkoutput(netif, p);
3. etharp_raw->netif->linkoutput(netif, p);




#ifndef MEM_ALIGNMENT
#define MEM_ALIGNMENT                   1
#endif



#ifndef LWIP_MEM_ALIGN_SIZE
#define LWIP_MEM_ALIGN_SIZE(size) (((size) + MEM_ALIGNMENT - 1) & ~(MEM_ALIGNMENT-1))
#endif

#define MEM_SIZE                        16000//
#define MEM_SIZE_ALIGNED     LWIP_MEM_ALIGN_SIZE(MEM_SIZE)

#define SIZEOF_STRUCT_MEM    LWIP_MEM_ALIGN_SIZE(sizeof(struct mem))

u8_t ram_heap[MEM_SIZE_ALIGNED + (2*SIZEOF_STRUCT_MEM) + MEM_ALIGNMENT];
#define LWIP_RAM_HEAP_POINTER ram_heap


#ifndef LWIP_MEM_ALIGN
#define LWIP_MEM_ALIGN(addr) ((void *)(((mem_ptr_t)(addr) + MEM_ALIGNMENT - 1) & ~(mem_ptr_t)(MEM_ALIGNMENT-1)))
#endif


 ram = (u8_t *)LWIP_MEM_ALIGN(LWIP_RAM_HEAP_POINTER);





 
 
 
 
 
 
 
 
 
 
 
 



	
	
	
	
	
	
	
	/*
 * SETUP: Make sure we define everything we will need.
 *
 * We have create three types of pools:
 *   1) MEMPOOL - standard pools
 *   2) MALLOC_MEMPOOL - to be used by mem_malloc in mem.c
 *   3) PBUF_MEMPOOL - a mempool of pbuf's, so include space for the pbuf struct
 *
 * If the include'r doesn't require any special treatment of each of the types
 * above, then will declare #2 & #3 to be just standard mempools.
 */
#ifndef LWIP_MALLOC_MEMPOOL
/* This treats "malloc pools" just like any other pool.
   The pools are a little bigger to provide 'size' as the amount of user data. */
#define LWIP_MALLOC_MEMPOOL(num, size) LWIP_MEMPOOL(POOL_##size, num, (size + LWIP_MEM_ALIGN_SIZE(sizeof(struct memp_malloc_helper))), "MALLOC_"#size)
#define LWIP_MALLOC_MEMPOOL_START
#define LWIP_MALLOC_MEMPOOL_END
#endif /* LWIP_MALLOC_MEMPOOL */ 

#ifndef LWIP_PBUF_MEMPOOL
/* This treats "pbuf pools" just like any other pool.
 * Allocates buffers for a pbuf struct AND a payload size */
#define LWIP_PBUF_MEMPOOL(name, num, payload, desc) LWIP_MEMPOOL(name, num, (MEMP_ALIGN_SIZE(sizeof(struct pbuf)) + MEMP_ALIGN_SIZE(payload)), desc)
#endif /* LWIP_PBUF_MEMPOOL */


/*
 * A list of internal pools used by LWIP.
 *
 * LWIP_MEMPOOL(pool_name, number_elements, element_size, pool_description)
 *     creates a pool name MEMP_pool_name. description is used in stats.c
 */
#if LWIP_RAW
LWIP_MEMPOOL(RAW_PCB,        MEMP_NUM_RAW_PCB,         sizeof(struct raw_pcb),        "RAW_PCB")
#endif /* LWIP_RAW */

#if LWIP_UDP
LWIP_MEMPOOL(UDP_PCB,        MEMP_NUM_UDP_PCB,         sizeof(struct udp_pcb),        "UDP_PCB")
#endif /* LWIP_UDP */

#if LWIP_TCP
LWIP_MEMPOOL(TCP_PCB,        MEMP_NUM_TCP_PCB,         sizeof(struct tcp_pcb),        "TCP_PCB")
LWIP_MEMPOOL(TCP_PCB_LISTEN, MEMP_NUM_TCP_PCB_LISTEN,  sizeof(struct tcp_pcb_listen), "TCP_PCB_LISTEN")
LWIP_MEMPOOL(TCP_SEG,        MEMP_NUM_TCP_SEG,         sizeof(struct tcp_seg),        "TCP_SEG")
#endif /* LWIP_TCP */

#if IP_REASSEMBLY
LWIP_MEMPOOL(REASSDATA,      MEMP_NUM_REASSDATA,       sizeof(struct ip_reassdata),   "REASSDATA")
#endif /* IP_REASSEMBLY */
#if (IP_FRAG && !IP_FRAG_USES_STATIC_BUF && !LWIP_NETIF_TX_SINGLE_PBUF) || LWIP_IPV6_FRAG
LWIP_MEMPOOL(FRAG_PBUF,      MEMP_NUM_FRAG_PBUF,       sizeof(struct pbuf_custom_ref),"FRAG_PBUF")
#endif /* IP_FRAG && !IP_FRAG_USES_STATIC_BUF && !LWIP_NETIF_TX_SINGLE_PBUF */

#if LWIP_NETCONN
LWIP_MEMPOOL(NETBUF,         MEMP_NUM_NETBUF,          sizeof(struct netbuf),         "NETBUF")
LWIP_MEMPOOL(NETCONN,        MEMP_NUM_NETCONN,         sizeof(struct netconn),        "NETCONN")
#endif /* LWIP_NETCONN */

//?1???é?¤
#if NO_SYS==0//
LWIP_MEMPOOL(TCPIP_MSG_API,  0,   sizeof(struct tcpip_msg),      "TCPIP_MSG_API")//MEMP_NUM_TCPIP_MSG_API
#if LWIP_MPU_COMPATIBLE
LWIP_MEMPOOL(API_MSG,        0,         sizeof(struct api_msg),        "API_MSG")//MEMP_NUM_API_MSG
#if LWIP_DNS
LWIP_MEMPOOL(DNS_API_MSG,    0,     sizeof(struct dns_api_msg),    "DNS_API_MSG")//MEMP_NUM_DNS_API_MSG
#endif
#if LWIP_SOCKET
LWIP_MEMPOOL(SOCKET_SETGETSOCKOPT_DATA, MEMP_NUM_SOCKET_SETGETSOCKOPT_DATA, sizeof(struct lwip_setgetsockopt_data), "SOCKET_SETGETSOCKOPT_DATA")
#endif
#if LWIP_NETIF_API
LWIP_MEMPOOL(NETIFAPI_MSG,   MEMP_NUM_NETIFAPI_MSG,    sizeof(struct netifapi_msg),   "NETIFAPI_MSG")
#endif
#endif /* LWIP_MPU_COMPATIBLE */
#if !LWIP_TCPIP_CORE_LOCKING_INPUT
LWIP_MEMPOOL(TCPIP_MSG_INPKT,0, sizeof(struct tcpip_msg),      "TCPIP_MSG_INPKT")//MEMP_NUM_TCPIP_MSG_INPKT
#endif /* !LWIP_TCPIP_CORE_LOCKING_INPUT */
#endif /* NO_SYS==0 */

#if LWIP_ARP && ARP_QUEUEING
LWIP_MEMPOOL(ARP_QUEUE,      MEMP_NUM_ARP_QUEUE,       sizeof(struct etharp_q_entry), "ARP_QUEUE")
#endif /* LWIP_ARP && ARP_QUEUEING */

#if LWIP_IGMP
LWIP_MEMPOOL(IGMP_GROUP,     MEMP_NUM_IGMP_GROUP,      sizeof(struct igmp_group),     "IGMP_GROUP")
#endif /* LWIP_IGMP */


#if (!NO_SYS || (NO_SYS && !NO_SYS_NO_TIMERS)) /* LWIP_TIMERS */
LWIP_MEMPOOL(SYS_TIMEOUT,    MEMP_NUM_SYS_TIMEOUT,     sizeof(struct sys_timeo),      "SYS_TIMEOUT")//
#endif /* LWIP_TIMERS */



#if LWIP_SNMP
LWIP_MEMPOOL(SNMP_ROOTNODE,  MEMP_NUM_SNMP_ROOTNODE,   sizeof(struct mib_list_rootnode), "SNMP_ROOTNODE")
LWIP_MEMPOOL(SNMP_NODE,      MEMP_NUM_SNMP_NODE,       sizeof(struct mib_list_node),     "SNMP_NODE")
LWIP_MEMPOOL(SNMP_VARBIND,   MEMP_NUM_SNMP_VARBIND,    sizeof(struct snmp_varbind),      "SNMP_VARBIND")
LWIP_MEMPOOL(SNMP_VALUE,     MEMP_NUM_SNMP_VALUE,      SNMP_MAX_VALUE_SIZE,              "SNMP_VALUE")
#endif /* LWIP_SNMP */
#if LWIP_DNS && LWIP_SOCKET
LWIP_MEMPOOL(NETDB,          MEMP_NUM_NETDB,           NETDB_ELEM_SIZE,               "NETDB")
#endif /* LWIP_DNS && LWIP_SOCKET */
#if LWIP_DNS && DNS_LOCAL_HOSTLIST && DNS_LOCAL_HOSTLIST_IS_DYNAMIC
LWIP_MEMPOOL(LOCALHOSTLIST,  MEMP_NUM_LOCALHOSTLIST,   LOCALHOSTLIST_ELEM_SIZE,       "LOCALHOSTLIST")
#endif /* LWIP_DNS && DNS_LOCAL_HOSTLIST && DNS_LOCAL_HOSTLIST_IS_DYNAMIC */
#if PPP_SUPPORT && PPPOE_SUPPORT
LWIP_MEMPOOL(PPPOE_IF,      MEMP_NUM_PPPOE_INTERFACES, sizeof(struct pppoe_softc),    "PPPOE_IF")
#endif /* PPP_SUPPORT && PPPOE_SUPPORT */

#if LWIP_IPV6 && LWIP_ND6_QUEUEING
LWIP_MEMPOOL(ND6_QUEUE,      MEMP_NUM_ND6_QUEUE,       sizeof(struct nd6_q_entry), "ND6_QUEUE")
#endif /* LWIP_IPV6 && LWIP_ND6_QUEUEING */

#if LWIP_IPV6 && LWIP_IPV6_REASS
LWIP_MEMPOOL(IP6_REASSDATA,      MEMP_NUM_REASSDATA,       sizeof(struct ip6_reassdata),   "IP6_REASSDATA")
#endif /* LWIP_IPV6 && LWIP_IPV6_REASS */

#if LWIP_IPV6 && LWIP_IPV6_MLD
LWIP_MEMPOOL(MLD6_GROUP,     MEMP_NUM_MLD6_GROUP,      sizeof(struct mld_group),     "MLD6_GROUP")
#endif /* LWIP_IPV6 && LWIP_IPV6_MLD */


/*
 * A list of pools of pbuf's used by LWIP.
 *
 * LWIP_PBUF_MEMPOOL(pool_name, number_elements, pbuf_payload_size, pool_description)
 *     creates a pool name MEMP_pool_name. description is used in stats.c
 *     This allocates enough space for the pbuf struct and a payload.
 *     (Example: pbuf_payload_size=0 allocates only size for the struct)
 */
LWIP_PBUF_MEMPOOL(PBUF,      MEMP_NUM_PBUF,            0,                             "PBUF_REF/ROM")
LWIP_PBUF_MEMPOOL(PBUF_POOL, PBUF_POOL_SIZE,           PBUF_POOL_BUFSIZE,             "PBUF_POOL")


/*
 * Allow for user-defined pools; this must be explicitly set in lwipopts.h
 * since the default is to NOT look for lwippools.h
 */
#if MEMP_USE_CUSTOM_POOLS
#include "lwippools.h"
#endif /* MEMP_USE_CUSTOM_POOLS */

/*
 * REQUIRED CLEANUP: Clear up so we don't get "multiply defined" error later
 * (#undef is ignored for something that is not defined)
 */
#undef LWIP_MEMPOOL
#undef LWIP_MALLOC_MEMPOOL
#undef LWIP_MALLOC_MEMPOOL_START
#undef LWIP_MALLOC_MEMPOOL_END
#undef LWIP_PBUF_MEMPOOL

	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
static u8_t *ram;
/** the last entry, always unused! */
static struct mem *ram_end;
/** pointer to the lowest free block, this is used for faster search */
static struct mem *lfree;
	
	
ram = (u8_t *)LWIP_MEM_ALIGN(LWIP_RAM_HEAP_POINTER);
/* initialize the start of the heap */
mem = (struct mem *)(void *)ram;
mem->next = MEM_SIZE_ALIGNED;
mem->prev = 0;
mem->used = 0;
/* initialize the end of the heap */
ram_end = (struct mem *)(void *)&ram[MEM_SIZE_ALIGNED];
ram_end->used = 1;
ram_end->next = MEM_SIZE_ALIGNED;
ram_end->prev = MEM_SIZE_ALIGNED;

/* initialize the lowest-free pointer to the start of the heap */
lfree = (struct mem *)(void *)ram;


struct mem {
  /** index (-> ram[next]) of the next struct */
  mem_size_t next;
  /** index (-> ram[prev]) of the previous struct */
  mem_size_t prev;
  /** 1: this area is used; 0: this area is unused */
  u8_t used;
};


void *mem_malloc(mem_size_t size)
{

  mem_size_t ptr, ptr2;
  struct mem *mem, *mem2;


    /* Scan through the heap searching for a free block that is big enough,
     * beginning with the lowest free block.
     */
    for (ptr = (mem_size_t)((u8_t *)lfree - ram); ptr < MEM_SIZE_ALIGNED - size;
         ptr = ((struct mem *)(void *)&ram[ptr])->next) {
      mem = (struct mem *)(void *)&ram[ptr];

      if ((!mem->used) &&
          (mem->next - (ptr + SIZEOF_STRUCT_MEM)) >= size) {
        /* mem is not used and at least perfect fit is possible:
         * mem->next - (ptr + SIZEOF_STRUCT_MEM) gives us the 'user data size' of mem */

        if (mem->next - (ptr + SIZEOF_STRUCT_MEM) >= (size + SIZEOF_STRUCT_MEM + MIN_SIZE_ALIGNED)) {
          /* (in addition to the above, we test if another struct mem (SIZEOF_STRUCT_MEM) containing
           * at least MIN_SIZE_ALIGNED of data also fits in the 'user data space' of 'mem')
           * -> split large block, create empty remainder,
           * remainder must be large enough to contain MIN_SIZE_ALIGNED data: if
           * mem->next - (ptr + (2*SIZEOF_STRUCT_MEM)) == size,
           * struct mem would fit in but no data between mem2 and mem2->next
           * @todo we could leave out MIN_SIZE_ALIGNED. We would create an empty
           *       region that couldn't hold data, but when mem->next gets freed,
           *       the 2 regions would be combined, resulting in more free memory
           */
          ptr2 = ptr + SIZEOF_STRUCT_MEM + size;
          /* create mem2 struct */
          mem2 = (struct mem *)(void *)&ram[ptr2];
          mem2->used = 0;
          mem2->next = mem->next;
          mem2->prev = ptr;
          /* and insert it between mem and mem->next */
          mem->next = ptr2;
          mem->used = 1;

          if (mem2->next != MEM_SIZE_ALIGNED) {
            ((struct mem *)(void *)&ram[mem2->next])->prev = ptr2;
          }

        } else {
          /* (a mem2 struct does no fit into the user data space of mem and mem->next will always
           * be used at this point: if not we have 2 unused structs in a row, plug_holes should have
           * take care of this).
           * -> near fit or excact fit: do not split, no mem2 creation
           * also can't move mem->next directly behind mem, since mem->next
           * will always be used at this point!
           */
          mem->used = 1;
        }

        if (mem == lfree) {
          /* Find next free block after mem and update lowest free pointer */
          while (lfree->used && lfree != ram_end) {
            
            lfree = (struct mem *)(void *)&ram[lfree->next];
          }
        
        }
        return (u8_t *)mem + SIZEOF_STRUCT_MEM;
      }
    }
  
  return NULL;

}

















void
mem_free(void *rmem)
{
  struct mem *mem;
 
  /* Get the corresponding struct mem ... */
  mem = (struct mem *)(void *)((u8_t *)rmem - SIZEOF_STRUCT_MEM);
  /* ... and is now unused. */
  mem->used = 0;

  if (mem < lfree) {
    /* the newly freed struct is now the lowest */
    lfree = mem;
  }



  /* finally, see if prev or next are free also */
  plug_holes(mem);
}



static void
plug_holes(struct mem *mem)
{
  struct mem *nmem;
  struct mem *pmem;

 
  nmem = (struct mem *)(void *)&ram[mem->next];
  if (mem != nmem && nmem->used == 0 && (u8_t *)nmem != (u8_t *)ram_end) {
    /* if mem->next is unused and not end of ram, combine mem and mem->next */
    if (lfree == nmem) {
      lfree = mem;
    }
    mem->next = nmem->next;
    ((struct mem *)(void *)&ram[nmem->next])->prev = (mem_size_t)((u8_t *)mem - ram);
  }

  /* plug hole backward */
  pmem = (struct mem *)(void *)&ram[mem->prev];
  if (pmem != mem && pmem->used == 0) {
    /* if mem->prev is unused, combine mem and mem->prev */
    if (lfree == mem) {
      lfree = pmem;
    }
    pmem->next = mem->next;
    ((struct mem *)(void *)&ram[mem->next])->prev = (mem_size_t)((u8_t *)pmem - ram);
  }
}






struct memp {
  struct memp *next;
};


void
memp_init(void)
{
  struct memp *memp;
  u16_t i, j;


  memp = (struct memp *)LWIP_MEM_ALIGN(memp_memory);  
  /* for every pool: */
  for (i = 0; i < MEMP_MAX; ++i) {
    memp_tab[i] = NULL;

    /* create a linked list of memp elements */
    for (j = 0; j < memp_num[i]; ++j) {
      memp->next = memp_tab[i];
      memp_tab[i] = memp;
      memp = (struct memp *)(void *)((u8_t *)memp + MEMP_SIZE + memp_sizes[i]
      );
    }
  }
}




void *
memp_malloc(memp_t type)
{
  struct memp *memp;
  memp = memp_tab[type];
  if (memp != NULL) {
    memp_tab[type] = memp->next;
    memp = (struct memp*)(void *)((u8_t*)memp + MEMP_SIZE);
  } 
  return memp;
}




void
memp_free(memp_t type, void *mem)
{
  struct memp *memp;

  memp = (struct memp *)(void *)((u8_t*)mem - MEMP_SIZE);
  
  memp->next = memp_tab[type]; 
  memp_tab[type] = memp;

}





struct pbuf *
pbuf_alloc(pbuf_layer layer, u16_t length, pbuf_type type)
{
  struct pbuf *p, *q, *r;
  u16_t offset;
  s32_t rem_len; /* remaining length */


  /* determine header offset */
  offset = 0;
  switch (layer) {
  case PBUF_TRANSPORT:
    /* add room for transport (often TCP) layer header */
    offset += PBUF_TRANSPORT_HLEN;
    /* FALLTHROUGH */
  case PBUF_IP:
    /* add room for IP layer header */
    offset += PBUF_IP_HLEN;
    /* FALLTHROUGH */
  case PBUF_LINK:
    /* add room for link layer header */
    offset += PBUF_LINK_HLEN;
    break;
  case PBUF_RAW:
    break;
  default:
    LWIP_ASSERT("pbuf_alloc: bad pbuf layer", 0);
    return NULL;
  }

  switch (type) {
  case PBUF_POOL:
    /* allocate head of pbuf chain into p */
    p = (struct pbuf *)memp_malloc(MEMP_PBUF_POOL);
    p->type = type;
    p->next = NULL;

    /* make the payload pointer point 'offset' bytes into pbuf data memory */
    p->payload = LWIP_MEM_ALIGN((void *)((u8_t *)p + (SIZEOF_STRUCT_PBUF + offset)));

    p->tot_len = length;
    /* set the length of the first pbuf in the chain */
    p->len = LWIP_MIN(length, PBUF_POOL_BUFSIZE_ALIGNED - LWIP_MEM_ALIGN_SIZE(offset));
    /* set reference count (needed here in case we fail) */
    p->ref = 1;

    /* now allocate the tail of the pbuf chain */

    /* remember first pbuf for linkage in next iteration */
    r = p;
    /* remaining length to be allocated */
    rem_len = length - p->len;
    /* any remaining pbufs to be allocated? */
    while (rem_len > 0) {
      q = (struct pbuf *)memp_malloc(MEMP_PBUF_POOL);
     
      q->type = type;
      q->flags = 0;
      q->next = NULL;
      /* make previous pbuf point to this pbuf */
      r->next = q;
      /* set total length of this pbuf and next in chain */
      
      q->tot_len = (u16_t)rem_len;
      /* this pbuf length is pool size, unless smaller sized tail */
      q->len = LWIP_MIN((u16_t)rem_len, PBUF_POOL_BUFSIZE_ALIGNED);
      q->payload = (void *)((u8_t *)q + SIZEOF_STRUCT_PBUF);
      
      q->ref = 1;
      /* calculate remaining length to be allocated */
      rem_len -= q->len;
      /* remember this pbuf for linkage in next iteration */
      r = q;
    }
    /* end of chain */
    /*r->next = NULL;*/

    break;
  case PBUF_RAM:
    /* If pbuf is to be allocated in RAM, allocate memory for it. */
    p = (struct pbuf*)mem_malloc(LWIP_MEM_ALIGN_SIZE(SIZEOF_STRUCT_PBUF + offset) + LWIP_MEM_ALIGN_SIZE(length));
    if (p == NULL) {
      return NULL;
    }
    /* Set up internal structure of the pbuf. */
    p->payload = LWIP_MEM_ALIGN((void *)((u8_t *)p + SIZEOF_STRUCT_PBUF + offset));
    p->len = p->tot_len = length;
    p->next = NULL;
    p->type = type;

    
    break;
  /* pbuf references existing (non-volatile static constant) ROM payload? */
  case PBUF_ROM:
  /* pbuf references existing (externally allocated) RAM payload? */
  case PBUF_REF:
    /* only allocate memory for the pbuf structure */
    p = (struct pbuf *)memp_malloc(MEMP_PBUF);
    
    /* caller must set this field properly, afterwards */
    p->payload = NULL;
    p->len = p->tot_len = length;
    p->next = NULL;
    p->type = type;
    break;
  default:
    
    return NULL;
  }
  /* set reference count */
  p->ref = 1;
  /* set flags */
  p->flags = 0;

  return p;
}



u8_t
pbuf_free(struct pbuf *p)
{
  u16_t type;
  struct pbuf *q;
  u8_t count;

 

  count = 0;
  /* de-allocate all consecutive pbufs from the head of the chain that
   * obtain a zero reference count after decrementing*/
  while (p != NULL) {
    u16_t ref;
    /* decrease reference count (number of pointers to pbuf) */
    ref = --(p->ref);

    if (ref == 0) {
      /* remember next pbuf in chain for next iteration */
      q = p->next;
      type = p->type;
      /* is this a custom pbuf? */
      if ((p->flags & PBUF_FLAG_IS_CUSTOM) != 0) {
        struct pbuf_custom *pc = (struct pbuf_custom*)p;
        pc->custom_free_function(p);
      } else

      {
        /* is this a pbuf from the pool? */
        if (type == PBUF_POOL) {
          memp_free(MEMP_PBUF_POOL, p);
        /* is this a ROM or RAM referencing pbuf? */
        } else if (type == PBUF_ROM || type == PBUF_REF) {
          memp_free(MEMP_PBUF, p);
        /* type == PBUF_RAM */
        } else {
          mem_free(p);
        }
      }
      count++;
      /* proceed to next pbuf */
      p = q;
    } else {   
      p = NULL;
    }
  }
  return count;
}

	
	
	
	
tcpip_timeout(1000, LwIP_DHCP_task, (void*)static_ip); ->>>>>>>>>>  
															err_t
															tcpip_timeout(u32_t msecs, sys_timeout_handler h, void *arg)
															{
															struct tcpip_msg *msg;
															if (sys_mbox_valid(&mbox)) {
															msg = (struct tcpip_msg *)memp_malloc(MEMP_TCPIP_MSG_API);
															if (msg == NULL) {
															  return ERR_MEM;
															}

															msg->type = TCPIP_MSG_TIMEOUT;
															msg->msg.tmo.msecs = msecs;
															msg->msg.tmo.h = h;
															msg->msg.tmo.arg = arg;
															sys_mbox_post(&mbox, msg);---->发送事件
															return ERR_OK;
														  }
															
	


static void
tcpip_thread(void *arg)
{

	switch (msg->type) 
	{
		......................
		
		case TCPIP_MSG_TIMEOUT:
			  LWIP_DEBUGF(TCPIP_DEBUG, ("tcpip_thread: TIMEOUT %p\n", (void *)msg));
			  sys_timeout(msg->msg.tmo.msecs, msg->msg.tmo.h, msg->msg.tmo.arg);-------->>>>>
			  memp_free(MEMP_TCPIP_MSG_API, msg);
			  break;
	}
}	

#define sys_timeout(msecs, handler, arg) sys_timeout_debug(msecs, handler, arg, #handler)
void
sys_timeout_debug(u32_t msecs, sys_timeout_handler handler, void *arg, const char* handler_name)
{
	struct sys_timeo *timeout, *t;

  timeout = (struct sys_timeo *)memp_malloc(MEMP_SYS_TIMEOUT);
 
  timeout->next = NULL;
  timeout->h = handler;
  timeout->arg = arg;
  timeout->time = msecs;
  timeout->handler_name = handler_name;
 

  if (next_timeout->time > msecs) {
    next_timeout->time -= msecs;
    timeout->next = next_timeout;
    next_timeout = timeout;
  } else {
    for(t = next_timeout; t != NULL; t = t->next) {
      timeout->time -= t->time;
      if (t->next == NULL || t->next->time > timeout->time) {
        if (t->next != NULL) {
          t->next->time -= timeout->time;
        }
        timeout->next = t->next;
        t->next = timeout;
        break;
      }
    }
  }

}








void
sys_timeout_debug(u32_t msecs, sys_timeout_handler handler, void *arg, const char* handler_name)
{
  struct sys_timeo *timeout, *t;

  timeout = (struct sys_timeo *)memp_malloc(MEMP_SYS_TIMEOUT);
  timeout->next = NULL;
  timeout->h = handler;
  timeout->arg = arg;
  timeout->time = msecs;

  if (next_timeout == NULL) {
    next_timeout = timeout;
    return;
  }

  if (next_timeout->time > msecs) {
    timeout->next = next_timeout;
    next_timeout = timeout;
  } else {
    for(t = next_timeout; t != NULL; t = t->next) {
        timeout->next = t->next;
        t->next = timeout;
        break;
      }
    }
  }
}


	while(1)
	{
		if((OSTimeGet() - Timerl) > OS_TICKS_PER_SEC * 5)
		{
			Timerl = OSTimeGet();
		}
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
	ipX_output(PCB_ISIPV6(pcb), seg->p, &pcb->local_ip, &pcb->remote_ip, pcb->ttl,
    pcb->tos, IP_PROTO_TCP);
	
	err_t
ip_output(struct pbuf *p, ip_addr_t *src, ip_addr_t *dest,
          u8_t ttl, u8_t tos, u8_t proto)
		




#define ip_addr_set(dest, src) ((dest)->addr = \
                                    ((src) == NULL ? 0 : \
                                    (src)->addr))		
		
#define ipX_addr_set(is_ipv6, dest, src)        ip_addr_set(dest, src)

	err_t
tcp_write(struct tcp_pcb *pcb, const void *arg, u16_t len, u8_t apiflags)
{
  struct pbuf *concat_p = NULL;
  struct tcp_seg *last_unsent = NULL, *seg = NULL, *prev_seg = NULL, *queue = NULL;
  u16_t pos = 0; /* position in 'arg' data */
  u16_t queuelen;
  u8_t optlen = 0;
  u8_t optflags = 0;
  u16_t oversize = 0;
  u16_t oversize_used = 0;

  err_t err;


  err = tcp_write_checks(pcb, len);
  if (err != ERR_OK) {
    return err;
  }
  queuelen = pcb->snd_queuelen;

  if (pcb->unsent != NULL) {
    u16_t space;
    u16_t unsent_optlen;

    /* @todo: this could be sped up by keeping last_unsent in the pcb */
    for (last_unsent = pcb->unsent; last_unsent->next != NULL;
         last_unsent = last_unsent->next);

    /* Usable space at the end of the last unsent segment */
    unsent_optlen = LWIP_TCP_OPT_LENGTH(last_unsent->flags);
    space = pcb->mss - (last_unsent->len + unsent_optlen);

    /*
     * Phase 1: Copy data directly into an oversized pbuf.
     *
     * The number of bytes copied is recorded in the oversize_used
     * variable. The actual copying is done at the bottom of the
     * function.
     */

    oversize = pcb->unsent_oversize;
    if (oversize > 0) {
      seg = last_unsent;
      oversize_used = oversize < len ? oversize : len;
      pos += oversize_used;
      oversize -= oversize_used;
      space -= oversize_used;
    }
  

    /*
     * Phase 2: Chain a new pbuf to the end of pcb->unsent.
     *
     * We don't extend segments containing SYN/FIN flags or options
     * (len==0). The new pbuf is kept in concat_p and pbuf_cat'ed at
     * the end.
     */
    if ((pos < len) && (space > 0) && (last_unsent->len > 0)) {
      u16_t seglen = space < len - pos ? space : len - pos;
      seg = last_unsent;

      /* Create a pbuf with a copy or reference to seglen bytes. We
       * can use PBUF_RAW here since the data appears in the middle of
       * a segment. A header will never be prepended. */
      if (apiflags & TCP_WRITE_FLAG_COPY) {
        /* Data is copied */
        if ((concat_p = tcp_pbuf_prealloc(PBUF_RAW, seglen, space, &oversize, pcb, apiflags, 1)) == NULL) {
          LWIP_DEBUGF(TCP_OUTPUT_DEBUG | 2,
                      ("tcp_write : could not allocate memory for pbuf copy size %"U16_F"\n",
                       seglen));
          goto memerr;
        }
        last_unsent->oversize_left = oversize;
        TCP_DATA_COPY2(concat_p->payload, (u8_t*)arg + pos, seglen, &concat_chksum, &concat_chksum_swapped);

      } else {
        /* Data is not copied */
        if ((concat_p = pbuf_alloc(PBUF_RAW, seglen, PBUF_ROM)) == NULL) {
          LWIP_DEBUGF(TCP_OUTPUT_DEBUG | 2,
                      ("tcp_write: could not allocate memory for zero-copy pbuf\n"));
          goto memerr;
        }

        /* reference the non-volatile payload data */
        concat_p->payload = (u8_t*)arg + pos;
      }

      pos += seglen;
      queuelen += pbuf_clen(concat_p);
    }
  }

  /*
   * Phase 3: Create new segments.
   *
   * The new segments are chained together in the local 'queue'
   * variable, ready to be appended to pcb->unsent.
   */
  while (pos < len) {
    struct pbuf *p;
    u16_t left = len - pos;
    u16_t max_len = pcb->mss - optlen;
    u16_t seglen = left > max_len ? max_len : left;

    if (apiflags & TCP_WRITE_FLAG_COPY) {
      /* If copy is set, memory should be allocated and data copied
       * into pbuf */
      if ((p = tcp_pbuf_prealloc(PBUF_TRANSPORT, seglen + optlen, pcb->mss, &oversize, pcb, apiflags, queue == NULL)) == NULL) {
        LWIP_DEBUGF(TCP_OUTPUT_DEBUG | 2, ("tcp_write : could not allocate memory for pbuf copy size %"U16_F"\n", seglen));
        goto memerr;
      }
      LWIP_ASSERT("tcp_write: check that first pbuf can hold the complete seglen",
                  (p->len >= seglen));
      TCP_DATA_COPY2((char *)p->payload + optlen, (u8_t*)arg + pos, seglen, &chksum, &chksum_swapped);
    } else {
      /* Copy is not set: First allocate a pbuf for holding the data.
       * Since the referenced data is available at least until it is
       * sent out on the link (as it has to be ACKed by the remote
       * party) we can safely use PBUF_ROM instead of PBUF_REF here.
       */
      struct pbuf *p2;

      if ((p2 = pbuf_alloc(PBUF_TRANSPORT, seglen, PBUF_ROM)) == NULL) {
        LWIP_DEBUGF(TCP_OUTPUT_DEBUG | 2, ("tcp_write: could not allocate memory for zero-copy pbuf\n"));
        goto memerr;
      }

      /* reference the non-volatile payload data */
      p2->payload = (u8_t*)arg + pos;

      /* Second, allocate a pbuf for the headers. */
      if ((p = pbuf_alloc(PBUF_TRANSPORT, optlen, PBUF_RAM)) == NULL) {
        /* If allocation fails, we have to deallocate the data pbuf as
         * well. */
        pbuf_free(p2);
        LWIP_DEBUGF(TCP_OUTPUT_DEBUG | 2, ("tcp_write: could not allocate memory for header pbuf\n"));
        goto memerr;
      }
      /* Concatenate the headers and data pbufs together. */
      pbuf_cat(p/*header*/, p2/*data*/);
    }

    queuelen += pbuf_clen(p);

    /* Now that there are more segments queued, we check again if the
     * length of the queue exceeds the configured maximum or
     * overflows. */
    if ((queuelen > TCP_SND_QUEUELEN) || (queuelen > TCP_SNDQUEUELEN_OVERFLOW)) {
      LWIP_DEBUGF(TCP_OUTPUT_DEBUG | 2, ("tcp_write: queue too long %"U16_F" (%"U16_F")\n", queuelen, TCP_SND_QUEUELEN));
      pbuf_free(p);
      goto memerr;
    }

    if ((seg = tcp_create_segment(pcb, p, 0, pcb->snd_lbb + pos, optflags)) == NULL) {
      goto memerr;
    }
    seg->oversize_left = oversize;


    /* first segment of to-be-queued data? */
    if (queue == NULL) {
      queue = seg;
    } else {
      /* Attach the segment to the end of the queued segments */
      LWIP_ASSERT("prev_seg != NULL", prev_seg != NULL);
      prev_seg->next = seg;
    }
    /* remember last segment of to-be-queued data for next iteration */
    prev_seg = seg;

    LWIP_DEBUGF(TCP_OUTPUT_DEBUG | LWIP_DBG_TRACE, ("tcp_write: queueing %"U32_F":%"U32_F"\n",
      ntohl(seg->tcphdr->seqno),
      ntohl(seg->tcphdr->seqno) + TCP_TCPLEN(seg)));

    pos += seglen;
  }

  /*
   * All three segmentation phases were successful. We can commit the
   * transaction.
   */

  /*
   * Phase 1: If data has been added to the preallocated tail of
   * last_unsent, we update the length fields of the pbuf chain.
   */

  if (oversize_used > 0) {
    struct pbuf *p;
    /* Bump tot_len of whole chain, len of tail */
    for (p = last_unsent->p; p; p = p->next) {
      p->tot_len += oversize_used;
      if (p->next == NULL) {
        TCP_DATA_COPY((char *)p->payload + p->len, arg, oversize_used, last_unsent);
        p->len += oversize_used;
      }
    }
    last_unsent->len += oversize_used;
  }
  pcb->unsent_oversize = oversize;


  /*
   * Phase 2: concat_p can be concatenated onto last_unsent->p
   */
  if (concat_p != NULL) {
    LWIP_ASSERT("tcp_write: cannot concatenate when pcb->unsent is empty",
      (last_unsent != NULL));
    pbuf_cat(last_unsent->p, concat_p);
    last_unsent->len += concat_p->tot_len;

  }

  /*
   * Phase 3: Append queue to pcb->unsent. Queue may be NULL, but that
   * is harmless
   */
  if (last_unsent == NULL) {
    pcb->unsent = queue;
  } else {
    last_unsent->next = queue;
  }

  /*
   * Finally update the pcb state.
   */
  pcb->snd_lbb += len;
  pcb->snd_buf -= len;
  pcb->snd_queuelen = queuelen;

  LWIP_DEBUGF(TCP_QLEN_DEBUG, ("tcp_write: %"S16_F" (after enqueued)\n",
    pcb->snd_queuelen));
  if (pcb->snd_queuelen != 0) {
    LWIP_ASSERT("tcp_write: valid queue length",
                pcb->unacked != NULL || pcb->unsent != NULL);
  }

  /* Set the PSH flag in the last segment that we enqueued. */
  if (seg != NULL && seg->tcphdr != NULL && ((apiflags & TCP_WRITE_FLAG_MORE)==0)) {
    TCPH_SET_FLAG(seg->tcphdr, TCP_PSH);
  }

  return ERR_OK;
memerr:
  pcb->flags |= TF_NAGLEMEMERR;
  TCP_STATS_INC(tcp.memerr);

  if (concat_p != NULL) {
    pbuf_free(concat_p);
  }
  if (queue != NULL) {
    tcp_segs_free(queue);
  }
  if (pcb->snd_queuelen != 0) {
    LWIP_ASSERT("tcp_write: valid queue length", pcb->unacked != NULL ||
      pcb->unsent != NULL);
  }
  LWIP_DEBUGF(TCP_QLEN_DEBUG | LWIP_DBG_STATE, ("tcp_write: %"S16_F" (with mem err)\n", pcb->snd_queuelen));
  return ERR_MEM;
}