#ifndef 	___FILEEEPROM_H__
#define		___FILEEEPROM_H__

#include "def.h"
#include "includes.h"
#define			CHIP_ADDR	(0XA0)

#define  		FLYEEPORM_TASK_START_STK_SIZE          100





#ifndef		_FLYEEPROMGOABLE_
	#define		FLYEEPROM_GLOBAL		extern
#else
	#define		FLYEEPROM_GLOBAL	

#endif


#define		MESSAGE_SIZE	256


typedef struct
{
	BYTE MessageBuf[MESSAGE_SIZE];
	UINT MessageBufHx;
	UINT MessageBufLx;
	UINT MessageBufOverflow;
	
	BYTE sendBuf[64];
	UINT sendBufLength;
	
	BYTE recBuf[64];
	UINT recBufLength;
	
	BYTE FrameBuf[MESSAGE_SIZE];
	UINT FrameBufLength;
	
	OS_EVENT *pFlyEEPORMSemEvent;
	
	
}T_FLYEEPROM_INFO;	
	
FLYEEPROM_GLOBAL T_FLYEEPROM_INFO flyeepromInfo;	
FLYEEPROM_GLOBAL UINT  GstkFlyEEPROM[FLYEEPORM_TASK_START_STK_SIZE];	
FLYEEPROM_GLOBAL void FlyEEPROMTaskCreate(void);
FLYEEPROM_GLOBAL void ipcEventProcFlyEEPROM(ULONG enumWhatEvent,ULONG lPara,BYTE *p,uint8_t length);
#endif	


