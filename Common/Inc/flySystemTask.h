#ifndef 	__FLYSYSTEM_H__
#define		__FLYSYSTEM_H__


#include "def.h"
#include "includes.h"
#define  		FLYSYSTEM_TASK_START_STK_SIZE          200


#ifndef			_FLYSYSTEMGOABLE_
	#define		FLYSYSTEM_EXTERN			extern				
#else
	#define		FLYSYSTEM_EXTERN	  	
#endif


typedef struct
{
	BYTE xxx;
	BOOL bIOSDDelectStatus;
}FLY_SYSTEM_INFO;

FLYSYSTEM_EXTERN OS_STK  GstkFlySYS[FLYSYSTEM_TASK_START_STK_SIZE];

FLYSYSTEM_EXTERN FLY_SYSTEM_INFO flySystemInfo;
	
FLYSYSTEM_EXTERN void ipcEventProcflySystem(ULONG enumWhatEvent,ULONG lPara,BYTE *p,uint8_t length);	
FLYSYSTEM_EXTERN void flySystemTaskCreate(void);
#endif

