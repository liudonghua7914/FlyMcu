#ifndef 	__SMALLTASK_H__
#define		__SMALLTASK_H__

#include "def.h"
#include "includes.h"
#define  		DEMO_TASK_START_STK_SIZE          150


#ifndef			_DEMOGOABLE_
	#define		DEMO_EXTERN			extern				
#else
	#define		DEMO_EXTERN	  	
#endif

typedef struct
{
	BYTE DemoCount;
}T_DEMO_INFO;	
	
DEMO_EXTERN T_DEMO_INFO demoInfo;
DEMO_EXTERN OS_STK  GstkDemo[DEMO_TASK_START_STK_SIZE];
DEMO_EXTERN void DemoDebugTaskCreate(void); 
DEMO_EXTERN	void ipcEventProcDemoDebug(ULONG enumWhatEvent,ULONG lPara,BYTE *p,uint8_t length);
#endif
