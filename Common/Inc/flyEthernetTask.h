#ifndef 	___FILETHERNET_H__
#define		___FILETHERNET_H__

#include "def.h"
#include "includes.h"


#define  		FLYETHERNET_TASK_START_STK_SIZE          100
#define 		RECSIZE			256

#define PP_HTONS(x) ((((x) & 0xff) << 8) | (((x) & 0xff00) >> 8))


#define ICMP_ER   0    /* echo reply */
#define ICMP_DUR  3    /* destination unreachable */
#define ICMP_SQ   4    /* source quench */
#define ICMP_RD   5    /* redirect */
#define ICMP_ECHO 8    /* echo */
#define ICMP_TE  11    /* time exceeded */
#define ICMP_PP  12    /* parameter problem */
#define ICMP_TS  13    /* timestamp */
#define ICMP_TSR 14    /* timestamp reply */
#define ICMP_IRQ 15    /* information request */
#define ICMP_IR  16    /* information reply */


#define ERR_OK  (0)
#define	ERR		(-1)
#define	NOERR	(0)


#define ETHTYPE_ARP       0x0806U
#define ETHTYPE_IP        0x0800U
#define ETHTYPE_VLAN      0x8100U
#define ETHTYPE_PPPOEDISC 0x8863U  /* PPP Over Ethernet Discovery Stage */
#define ETHTYPE_PPPOE     0x8864U  /* PPP Over Ethernet Session Stage */


#define SIZEOF_ETH_HDR (14)




#ifndef		_FLYETHERNETGOABLE_
	#define		FLYETHERNET_GLOBAL		extern
#else
	#define		FLYETHERNET_GLOBAL	

#endif

typedef enum {
  PBUF_RAM, /* pbuf data is stored in RAM */
  PBUF_ROM, /* pbuf data is stored in ROM */
  PBUF_REF, /* pbuf comes from the pbuf pool */
  PBUF_POOL /* pbuf payload refers to RAM */
} pbuf_type;

	
struct eth_addr 
{
  BYTE addr[6];
};	
	
struct eth_hdr
{
	BYTE DesAddr[6];
	BYTE SrcAddr[6];
	UINT16 Type;
};

struct etharp_hdr
{
	UINT16 HardType;
	UINT16 ProtocolType;
	BYTE HardLen;
	BYTE protocolLen;
	UINT16 Op;
	BYTE SrcMAC[6];
	BYTE SrcIP[4];
	BYTE DscMAC[6];
	BYTE DscIP[4];
};


struct ip_hdr
{
	UINT16 v_hl_tos;
	UINT16 len;
	UINT16 id;
	UINT16 offset;
	BYTE ttl;
	BYTE proto;
	UINT16 chksum;
	BYTE srcIP[4];
	BYTE dscIP[4];
};


typedef struct
{
	BYTE type;
	BYTE icode;
	UINT16 chksum;
	UINT16 id;
	UINT16 seqno;
}T_ICMP_INFO;


struct pbuf
{
	struct pbuf *next;
	void *payload;
	UINT16 tot_len;
	UINT16 len;
	BYTE  type;
	BYTE flags;
	UINT16 ref;
};

struct ip_addr 
{
	BYTE  addr[4];
};


typedef struct ip_addr ip_addr_t;


struct netif 
{
	struct netif *next;

	ip_addr_t ip_addr;
	ip_addr_t netmask;
	ip_addr_t gw;
	BYTE hwaddr[6];
};

typedef struct
{
	BYTE xxx;
	BYTE recBuf[RECSIZE];
	BYTE currentSrcIP[4];
	BYTE currentDesIP[4];
	BYTE currentSrcMac[6];
	BYTE currentDesMac[6];
	struct pbuf pbuf;
	void *playload;
	struct netif mynetif;
	
}T_FLYEHTERNET_INFO;	
FLYETHERNET_GLOBAL T_FLYEHTERNET_INFO flyEhternetInfo;
FLYETHERNET_GLOBAL UINT GstkFlyEthernet[FLYETHERNET_TASK_START_STK_SIZE];	
FLYETHERNET_GLOBAL void FlylyEthernetCreate(void);
FLYETHERNET_GLOBAL void ipcEventProcFlylyEthernet(ULONG enumWhatEvent,ULONG lPara,BYTE *p,uint8_t length);

FLYETHERNET_GLOBAL void WriteEthernetData(BYTE *p,UINT len);
#endif	


