#define		_FLYSYSTEMGOABLE_
#include "config.h"
#include "lpc17xx_lib.h"
#include "includes.h"
#include "UserType.h"


BYTE SendBuf[] = {"\r\nFlyAudio!!!FlyAudio!!!FlyAudio!!!FlyAudio!!!FlyAudio!!!FlyAudio!!!FlyAudio!!!FlyAudio!!!FlyAudio!!!FlyAudio!!!FlyAudio!!!FlyAudio!!!FlyAudio!!!FlyAudio!!!FlyAudio!!!FlyAudio!!!"};



/***************************************************************************************************************************
**函数名称:	 	checkIOSDDelectStatus
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void checkIOSDDelectStatus(void)
{
	
	if(flySystemInfo.bIOSDDelectStatus != IO_Read(IO_SDCARDCD))
	{
		flySystemInfo.bIOSDDelectStatus = IO_Read(IO_SDCARDCD);
		ipcStartEvent(EVENT_GLOBAL_FLY_FILE_SDSTATUS,flySystemInfo.bIOSDDelectStatus,NULL,0);
	}
}
/***************************************************************************************************************************
**函数名称:	 	checkIOStatus
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void checkIOStatus(void)
{
	checkIOSDDelectStatus();
}
/***************************************************************************************************************************
**函数名称:	 	ipcFlySystemInit
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void ipcFlySystemInit(void)
{
	flySystemTaskCreate();
	flySystemInfo.bIOSDDelectStatus = !IO_Read(IO_SDCARDCD);
}
/***************************************************************************************************************************
**函数名称:	 	ipcEventProcflySystem
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void ipcEventProcflySystem(ULONG enumWhatEvent,ULONG lPara,BYTE *p,uint8_t length)
{
	BOOL bRes = TRUE;
	
	switch(enumWhatEvent)
	{
		case EVENT_GLOBAL_MODULE_INIT:		ipcFlySystemInit();
											break;
											
		default:
											break;
	}
	
	if(bRes)
	{
		ipcClearEvent(enumWhatEvent);
	}
}
/***************************************************************************************************************************
**函数名称:	 	flySystemTask
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void flySystemTask(void *arg)
{
	while(1)
	{
		printf("\r\n flySystemTask");
		//checkIOStatus();
		OSTimeDly(OS_TICKS_PER_SEC / 2);  
	}
}
/***************************************************************************************************************************
**函数名称:	 	FlySystemTaskCreate
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void flySystemTaskCreate(void)
{
	INT8U Res;
	Res = OSTaskCreate(flySystemTask, 							//执行函数
				 NULL,	  										//带入的参数
				 &GstkFlySYS[FLYSYSTEM_TASK_START_STK_SIZE-1],	//堆栈由高地址往底地址增长
				 (INT8U)PRIO_FLYSYSTEM							//任务优先级
				 );
	if(OS_NO_ERR != Res)
	{
		printf("\r\n Res = %d",Res);
	}
}

