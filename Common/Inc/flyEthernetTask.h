#ifndef 	___FILETHERNET_H__
#define		___FILETHERNET_H__

#include "def.h"
#include "includes.h"


#define  		FLYETHERNET_TASK_START_STK_SIZE          100
#define 		RECSIZE			256

#define PP_HTONS(x) ((((x) & 0xff) << 8) | (((x) & 0xff00) >> 8))


#ifndef		_FLYETHERNETGOABLE_
	#define		FLYETHERNET_GLOBAL		extern
#else
	#define		FLYETHERNET_GLOBAL	

#endif

	
typedef struct
{
	BYTE DesAddr[6];
	BYTE SrcAddr[6];
	UINT16 Type;
}T_PACKET_INFO,*P_PACKTE_INFO;

typedef struct
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
}T_ARP_PACKET_INFO;

typedef struct
{
	BYTE xxx;
	BYTE recBuf[RECSIZE];
	
}T_FLYEHTERNET_INFO;	
FLYETHERNET_GLOBAL T_FLYEHTERNET_INFO flyEhternetInfo;
FLYETHERNET_GLOBAL UINT GstkFlyEthernet[FLYETHERNET_TASK_START_STK_SIZE];	
FLYETHERNET_GLOBAL void FlylyEthernetCreate(void);
FLYETHERNET_GLOBAL void ipcEventProcFlylyEthernet(ULONG enumWhatEvent,ULONG lPara,BYTE *p,uint8_t length);

FLYETHERNET_GLOBAL void WriteEthernetData(BYTE *p,UINT len);
#endif	


