#define  _FLYFILEGOABLE_

#include "config.h"
#include "lpc17xx_lib.h"
#include "includes.h"
#include "fs_include.h"
#include "UserType.h"

/***************************************************************************************************************************
**��������:	 	ipcFileSystemInit
**��������:	 	
**��ڲ���:
**���ز���:
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
**��������:	 	ipcEventProcflyFile
**��������:	 	
**��ڲ���:
**���ز���:
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
**��������:	 	FileSystemTask
**��������:	 	
**��ڲ���:
**���ز���:
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
**��������:	 	FileSystemTaskCreate
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
void flyFileTaskCreate(void)
{
	INT8U Res;
	Res = OSTaskCreate(flyFileTask, 							//ִ�к���
				 NULL,	  										//����Ĳ���
				 &GstkflyFile[FLYFILE_TASK_START_STK_SIZE-1],	//��ջ�ɸߵ�ַ���׵�ַ����
				 (INT8U)PRIO_FLYFILE							//�������ȼ�
				 );
	if(OS_NO_ERR != Res)
	{
		LIBMCU_DEBUG(FILE_DEBUG,("\r\n Res = %d",Res));
	}
	
}


