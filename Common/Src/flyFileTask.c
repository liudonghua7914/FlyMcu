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
	if(NULL ==  FS_FOpen("mmc:\\baidu_http.txt","r"))
	{
		LIBMCU_DEBUG(FILE_DEBUG,("\r\n FS_FOpen Fail"));
		//return FALSE;
	}
}
/***************************************************************************************************************************
**��������:	 	OperationFile
**��������:	 	
**��ڲ���:
**���ز���:
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


