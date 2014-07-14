#ifndef 	___FILETHERNET_H__
#define		___FILETHERNET_H__


#if 0
#include "def.h"
#include "includes.h"


#define  		FLYETHERNET_TASK_START_STK_SIZE          150
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


#define ICMP_TTL       (255)


#define ERR_OK          0    /* No error, everything OK. */
#define ERR_MEM        -1    /* Out of memory error.     */
#define ERR_BUF        -2    /* Buffer error.            */
#define ERR_TIMEOUT    -3    /* Timeout.                 */
#define ERR_RTE        -4    /* Routing problem.         */
#define ERR_INPROGRESS -5    /* Operation in progress    */
#define ERR_VAL        -6    /* Illegal value.           */
#define ERR_WOULDBLOCK -7    /* Operation would block.   */
#define ERR_USE        -8    /* Address in use.          */
#define ERR_ISCONN     -9    /* Already connected.       */

#define ERR_IS_FATAL(e) ((e) < ERR_ISCONN)

#define ERR_ABRT       -10   /* Connection aborted.      */
#define ERR_RST        -11   /* Connection reset.        */
#define ERR_CLSD       -12   /* Connection closed.       */
#define ERR_CONN       -13   /* Not connected.           */

#define ERR_ARG        -14   /* Illegal argument.        */

#define ERR_IF         -15   /* Low-level netif error    */


#define ETHTYPE_ARP       0x0806U
#define ETHTYPE_IP        0x0800U
#define ETHTYPE_VLAN      0x8100U
#define ETHTYPE_PPPOEDISC 0x8863U  /* PPP Over Ethernet Discovery Stage */
#define ETHTYPE_PPPOE     0x8864U  /* PPP Over Ethernet Session Stage */



#define	MAC0	0x1E
#define	MAC1	0x30
#define	MAC2	0x6c
#define	MAC3	0xa2
#define	MAC4	0x45
#define	MAC5	0x5E

#define FOLD_U32T(u)          (((u) >> 16) + ((u) & 0x0000ffffUL))
#define SWAP_BYTES_IN_WORD(w) (((w) & 0xff) << 8) | (((w) & 0xff00) >> 8)

#define SIZEOF_ETH_HDR (14)
#define PBUF_IP_HLEN   (20)

#define IP_HDRINCL  NULL

#ifndef		_FLYETHERNETGOABLE_
	#define		FLYETHERNET_GLOBAL		extern
#else
	#define		FLYETHERNET_GLOBAL	

#endif





struct eth_addr 
{
  BYTE addr[6];
};	

struct ip_addr 
{
	BYTE addr[4];
};


typedef struct ip_addr ip_addr_t;
typedef int err_t;	

typedef enum 
{
	PBUF_RAM, /* pbuf data is stored in RAM */
	PBUF_ROM, /* pbuf data is stored in ROM */
	PBUF_REF, /* pbuf comes from the pbuf pool */
	PBUF_POOL /* pbuf payload refers to RAM */
}pbuf_type;

	
struct icmp_echo_hdr 
{
	BYTE type;
	BYTE icode;
	UINT16 chksum;
	UINT16 id;
	UINT16 seqno;
};

struct eth_hdr
{
	struct eth_addr DesMac;
	struct eth_addr SrcMac;
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
	ip_addr_t srcIP;
	ip_addr_t dscIP;
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



struct netif 
{
	struct netif *next;

	ip_addr_t ip_addr;
	ip_addr_t netmask;
	ip_addr_t gw;
	struct eth_addr hwaddr;
};

typedef struct
{
	BYTE xxx;
	BYTE recBuf[RECSIZE];
	ip_addr_t currentSrcIP;
	ip_addr_t currentDesIP;
	struct eth_addr currentSrcMac;
	struct eth_addr currentDesMac;
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
#endif	


