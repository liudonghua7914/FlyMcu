#define		_FLYSYSTEMGOABLE_
#include "config.h"
#include "lpc17xx_lib.h"
#include "includes.h"
#include "UserType.h"


BYTE SendBuf[] = {"\r\nFlyAudio!!!FlyAudio!!!FlyAudio!!!FlyAudio!!!FlyAudio!!!FlyAudio!!!FlyAudio!!!FlyAudio!!!FlyAudio!!!FlyAudio!!!FlyAudio!!!FlyAudio!!!FlyAudio!!!FlyAudio!!!FlyAudio!!!FlyAudio!!!"};



/***************************************************************************************************************************
**��������:	 	checkIOSDDelectStatus
**��������:	 	
**��ڲ���:
**���ز���:
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
**��������:	 	checkIOStatus
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
void checkIOStatus(void)
{
	checkIOSDDelectStatus();
}
/***************************************************************************************************************************
**��������:	 	ipcFlySystemInit
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
void ipcFlySystemInit(void)
{
	flySystemTaskCreate();
	flySystemInfo.bIOSDDelectStatus = !IO_Read(IO_SDCARDCD);
}
/***************************************************************************************************************************
**��������:	 	ipcEventProcflySystem
**��������:	 	
**��ڲ���:
**���ز���:
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
**��������:	 	flySystemTask
**��������:	 	
**��ڲ���:
**���ز���:
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
**��������:	 	FlySystemTaskCreate
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
void flySystemTaskCreate(void)
{
	INT8U Res;
	Res = OSTaskCreate(flySystemTask, 							//ִ�к���
				 NULL,	  										//����Ĳ���
				 &GstkFlySYS[FLYSYSTEM_TASK_START_STK_SIZE-1],	//��ջ�ɸߵ�ַ���׵�ַ����
				 (INT8U)PRIO_FLYSYSTEM							//�������ȼ�
				 );
	if(OS_NO_ERR != Res)
	{
		printf("\r\n Res = %d",Res);
	}
}

