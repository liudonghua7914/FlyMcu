#define  _DEMOGOABLE_
#include "config.h"
#include "lpc17xx_lib.h"
#include "includes.h"
#include "UserType.h"

/***************************************************************************************************************************
**函数名称:	 	DemoEEPROM
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void DemoEEPROM(void)
{
	static BYTE buf[] = {0X07,0X01,0X00};
	buf[2] = demoInfo.DemoCount;
	ipcStartEvent(EVENT_GLOBAL_FLY_EEPROM_CMD,0,buf,3);
	demoInfo.DemoCount = (demoInfo.DemoCount + 1) % 256;
}
/***************************************************************************************************************************
**函数名称:	 	ipcDemoDebugInit
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void ipcDemoDebugInit(void)
{
	DemoDebugTaskCreate();
}
/***************************************************************************************************************************
**函数名称:	 	ipcEventProcDemoDebug
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void ipcEventProcDemoDebug(ULONG enumWhatEvent,ULONG lPara,BYTE *p,uint8_t length)
{
	BOOL bRes = TRUE;
	switch(enumWhatEvent)
	{
		case EVENT_GLOBAL_MODULE_INIT:		ipcDemoDebugInit();
											printf("\r\n ipcDemoDebugInit OK");
											break;
		
		
		default:							break;
	}
	
	if(bRes)
	{
		ipcClearEvent(enumWhatEvent);
	}
}
/***************************************************************************************************************************
**函数名称:	 	DemoDebugTask
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void DemoDebugTask(void  *parg)
{
	while(1)
	{
//		printf("\r\n=============================================\r\n");
//		printf("\r\n IO_SDCARDCD = %d",IO_Read(IO_SDCARDCD));
//		printf("\r\n=============================================\r\n");
//		DemoEEPROM();
	   	OSTimeDly(OS_TICKS_PER_SEC);  
	}
}
/***************************************************************************************************************************
**函数名称:	 	DemoDebugTaskCreate
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void DemoDebugTaskCreate(void)
{

	OSTaskCreate(DemoDebugTask, 						//执行函数
				 NULL,	  								//带入的参数
				 &GstkDemo[DEMO_TASK_START_STK_SIZE-1],	//堆栈由高地址往底地址增长
				 (INT8U)PRIO_DEMO						//任务优先级
				 );
	
}


