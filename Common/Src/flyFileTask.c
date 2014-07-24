#define  _FLYFILEGOABLE_

#include "config.h"
#include "lpc17xx_lib.h"
#include "includes.h"
#include "fs_include.h"
#include "UserType.h"

/***************************************************************************************************************************
**函数名称:	 	ipcFileSystemInit
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void ipcFileSystemInit(void)
{
	flyFileTaskCreate();
	flyFileInfo.fp = NULL;
	flyFileInfo.fp = FS_FOpen("mmc:\\test.txt","ab+");
	if(NULL == flyFileInfo.fp)
	{
		LIBMCU_DEBUG(FILE_DEBUG,("\r\n FS_FOpen Fail"));
	}
	LIBMCU_DEBUG(FILE_DEBUG,("\r\n ipcFileSystemInit OK"));
}
/***************************************************************************************************************************
**函数名称:	 	ipcEventProcflyFile
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void ipcEventProcflyFile(ULONG enumWhatEvent,ULONG lPara,BYTE *p,UINT length)
{
	BOOL bRes = TRUE;
	
	switch(enumWhatEvent)
	{
		case EVENT_GLOBAL_MODULE_INIT:		ipcFileSystemInit();					
											break;
		
		case EVENT_GLOBAL_FLY_FILE_SDSTATUS:
											if(lPara)
											{	
												FS_FClose(flyFileInfo.fp);
												flyFileInfo.fp = NULL;
											}	
											break;
			
		case EVENT_GLOBAL_FLY_FILE_WRITE:	if(flyFileInfo.fp)
											{									
												LIBMCU_DEBUG(FILE_DEBUG,("\r\n Write: %d ",length));
												FS_FWrite(p,1,length,flyFileInfo.fp);
											}
											break;		
		
		case EVENT_GLOBAL_FLY_FILE_READ:	if(flyFileInfo.fp)
											{									
												LIBMCU_DEBUG(FILE_DEBUG,("\r\n Read: %d ",length));
												FS_FRead(p,1,length,flyFileInfo.fp);
											}
											break;		
																	
		default:							break;
	}
	
	if(bRes)
	{
		ipcClearEvent(enumWhatEvent);
	}
}
/***************************************************************************************************************************
**函数名称:	 	FileSystemTask
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void flyFileTask(void  *parg)
{
	UINT len;
	while(1)
	{		
		OSTimeDly(OS_TICKS_PER_SEC);  
	}
}
/***************************************************************************************************************************
**函数名称:	 	FileSystemTaskCreate
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void flyFileTaskCreate(void)
{
	INT8U Res;
	Res = OSTaskCreate(flyFileTask, 							//执行函数
				 NULL,	  										//带入的参数
				 &GstkflyFile[FLYFILE_TASK_START_STK_SIZE-1],	//堆栈由高地址往底地址增长
				 (INT8U)PRIO_FLYFILE							//任务优先级
				 );
	if(OS_NO_ERR != Res)
	{
		LIBMCU_DEBUG(FILE_DEBUG,("\r\n Res = %d",Res));
	}
	
}


