#ifndef 	___FILETHERNET_H__
#define		___FILETHERNET_H__


#include "def.h"
#include "includes.h"


#include "api.h"
#include "tcpip.h"
#include "udp.h"
#include "tcp.h"
#include "netif.h"
//#include "Ethernetif.h"
#include "fs_include.h"

#ifndef		_FLYETHERNETGOABLE_
	#define		FLYETHERNET_GLOBAL		extern
#else
	#define		FLYETHERNET_GLOBAL	

#endif


#define  		LWIP_TASK_START_STK_SIZE         		250
#define  		FLYETHERNET_TASK_START_STK_SIZE         250
#define 		RECSIZE			512

#define	 MAC0		0X1e
#define	 MAC1		0x30
#define	 MAC2		0x6c
#define	 MAC3		0xa2
#define	 MAC4		0x45
#define	 MAC5		0x5E	



typedef struct
{
	BYTE xxx;
	BYTE recBuf[RECSIZE];

	struct ip_addr ipaddr;
	struct ip_addr netmask;
	struct ip_addr gw;
	
	
	struct netconn *pNetconn;
	struct netconn *pNewnetconn;
	struct netif netif;
	struct netbuf *pNetbuf;
	FS_FILE *httpfd;
	OS_EVENT *LwIPSem;
}T_FLYEHTERNET_INFO;	


FLYETHERNET_GLOBAL T_FLYEHTERNET_INFO flyEhternetInfo;
FLYETHERNET_GLOBAL UINT GstkFlyEthernet[FLYETHERNET_TASK_START_STK_SIZE];	
FLYETHERNET_GLOBAL UINT GstkLwip[FLYETHERNET_TASK_START_STK_SIZE];
FLYETHERNET_GLOBAL void FlyEthernetCreate(void);
FLYETHERNET_GLOBAL void LwipTaskCreate(void);
FLYETHERNET_GLOBAL void ipcEventProcFlylyEthernet(ULONG enumWhatEvent,ULONG lPara,BYTE *p,uint8_t length);
FLYETHERNET_GLOBAL BOOL OpenMMCFile(char *name);
#endif



