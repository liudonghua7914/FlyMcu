#ifndef 	___FILETHERNET_H__
#define		___FILETHERNET_H__

#include "def.h"
#include "includes.h"


#define  		FLYETHERNET_TASK_START_STK_SIZE          100
#define 		RECSIZE			256




#ifndef		_FLYETHERNETGOABLE_
	#define		FLYETHERNET_GLOBAL		extern
#else
	#define		FLYETHERNET_GLOBAL	

#endif



typedef struct
{
	BYTE xxx;
	BYTE recBuf[256];
	
}T_FLYEHTERNET_INFO;	
FLYETHERNET_GLOBAL T_FLYEHTERNET_INFO flyEhternetInfo;
FLYETHERNET_GLOBAL UINT GstkFlyEthernet[FLYETHERNET_TASK_START_STK_SIZE];	
FLYETHERNET_GLOBAL void FlylyEthernetCreate(void);
FLYETHERNET_GLOBAL void ipcEventProcFlylyEthernet(ULONG enumWhatEvent,ULONG lPara,BYTE *p,uint8_t length);
#endif	


