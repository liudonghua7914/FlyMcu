#define  _FILESYSTEMGOABLE_

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
	UINT i;
	FileSystemTaskCreate();

	printf("\r\n ipcFileSystemInit OK");
}
/***************************************************************************************************************************
**函数名称:	 	ipcEventProcFileSystem
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void ipcEventProcFileSystem(ULONG enumWhatEvent,ULONG lPara,BYTE *p,uint8_t length)
{
	switch(enumWhatEvent)
	{
		case EVENT_GLOBAL_MODULE_INIT:		ipcFileSystemInit();
											fileSystemInfo.fp = NULL;
											break;
		
		case EVENT_GLOBAL_OPEN_FILE:		
											if(NULL == fileSystemInfo.fp)
											{
												fileSystemInfo.fp = FS_FOpen("mmc:\\test.txt","rb");
												if(NULL == fileSystemInfo.fp)
												{
													printf("\r\n FOpen Fail");	
												}
												else
												{
													printf("\r\n FOpen OK");
													FS_FSeek(fileSystemInfo.fp,0,FS_SEEK_END);
													fileSystemInfo.FileLength  = FS_FTell(fileSystemInfo.fp);
													FS_FSeek(fileSystemInfo.fp,0,FS_SEEK_SET);
													printf("\r\n FileLength = %d ",fileSystemInfo.FileLength);
												}
											}
											
											break;
		
		
		
		default:							break;
	}
}

/***************************************************************************************************************************
**函数名称:	 	FileSystemTask
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void FileSystemTask(void  *parg)
{
	while(1)
	{
	   //	fileSystemSDCardStatusProc();
		OSTimeDly(OS_TICKS_PER_SEC / 10);  
	}
}


/***************************************************************************************************************************
**函数名称:	 	FileSystemTaskCreate
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void FileSystemTaskCreate(void)
{
	INT8U Res;
	Res = OSTaskCreate(FileSystemTask, 								//执行函数
				 NULL,	  										//带入的参数
				 &GstkFileSYS[FILESYSTE_TASK_START_STK_SIZE-1],	//堆栈由高地址往底地址增长
				 (INT8U)PRIO_FILESYSTE							//任务优先级
				 );
	if(OS_NO_ERR != Res)
	{
		printf("\r\n Res = %d",Res);
	}
	
}


