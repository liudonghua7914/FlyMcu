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


	
typedef unsigned char  DataType;	
	
typedef struct
{
	BYTE DemoCount;
	UINT32 DemoDebugTickTime;
	OS_EVENT *pDemoDebugEvent;
	DataType DemoDebug[2048];
}T_DEMO_INFO;	
	
DEMO_EXTERN T_DEMO_INFO demoInfo;
DEMO_EXTERN OS_STK  GstkDemo[DEMO_TASK_START_STK_SIZE];
DEMO_EXTERN void DemoDebugTaskCreate(void); 
DEMO_EXTERN	void ipcEventProcDemoDebug(ULONG enumWhatEvent,ULONG lPara,BYTE *p,uint8_t length);
DEMO_EXTERN void printf_w(const char *format, ...);
DEMO_EXTERN void DemofifoInit(void);
#endif
