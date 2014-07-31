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
	if(NULL ==  FS_FOpen("mmc:\\baidu_http.txt","r"))
	{
		LIBMCU_DEBUG(FILE_DEBUG,("\r\n FS_FOpen Fail"));
		//return FALSE;
	}
}
/***************************************************************************************************************************
**函数名称:	 	OperationFile
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void OperationFile(BYTE eStatus,BYTE *p,UINT len)
{
	BYTE filenamelen = 0;
	switch(eStatus)
	{
		case eOpen:		
											
						break;
						
		case eRead:
						break;
		case eWrite:
						break;
		case eClose:
						break;
		default:
						break;
	}
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
		case EVENT_GLOBAL_MODULE_INIT:			ipcFileSystemInit();					
												break;
		
		case EVENT_GLOBAL_FLY_FILE_SDSTATUS:
											
			
		case EVENT_GLOBAL_FLY_FILE_OPERATION:	OperationFile((BYTE)lPara,p,length);
											
												break;		
		
		
		default:								break;
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


