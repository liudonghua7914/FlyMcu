#define		_FLYETHERNETGOABLE_

#include "config.h"
#include "lpc17xx_lib.h"
#include "includes.h"
#include "UserType.h"


/***************************************************************************************************************************
**��������:	 	ipcFlyEthernetInit
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
void ipcFlyEthernetInit(void)
{
	FlylyEthernetCreate();
	printf("\r\n ipcFlyEthernetInit OK");
}
/***************************************************************************************************************************
**��������:	 	ipcEventProcFlylyEthernet
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
void ipcEventProcFlylyEthernet(ULONG enumWhatEvent,ULONG lPara,BYTE *p,uint8_t length)
{
	BOOL bRes = TRUE;
	switch(enumWhatEvent)
	{
		case EVENT_GLOBAL_MODULE_INIT:		ipcFlyEthernetInit();				
											break;
		
		default:							break;
	}
	
	if(bRes)
	{
		ipcClearEvent(enumWhatEvent);
	}
}

/***************************************************************************************************************************
**��������:	 	ReadEthernetData
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
void ReadEthernetData(void)
{
	UINT len;
	if (LPC_EMAC->RxProduceIndex == LPC_EMAC->RxConsumeIndex) 
	{
	  return;
	}
	
	printf("LDH RxConsumeIndex = %d \n",LPC_EMAC->RxConsumeIndex);
	printf("LDH RxProduceIndex = %d \n",LPC_EMAC->RxProduceIndex);
	len = (RX_STAT_INFO(LPC_EMAC->RxConsumeIndex) & RINFO_SIZE) - 1;
	
}
/***************************************************************************************************************************
**��������:	 	FlyEthernetTask
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
void FlyEthernetTask(void *arg)
{
	INT8U Error;
	while(1)
	{
		ReadEthernetData();
		OSTimeDly(OS_TICKS_PER_SEC/10);  	
	}
}
/***************************************************************************************************************************
**��������:	 	FlylyEthernetCreate
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
void FlylyEthernetCreate(void)
{
	INT8U Res;
	Res = OSTaskCreate(FlyEthernetTask, 								//ִ�к���
				 NULL,	  											//����Ĳ���
				 &GstkFlyEthernet[FLYETHERNET_TASK_START_STK_SIZE-1],	//��ջ�ɸߵ�ַ���׵�ַ����
				 (INT8U)PRIO_FLYEHTERNET								//�������ȼ�
				 );
	if(OS_NO_ERR != Res)
	{
		printf("\r\n Res = %d",Res);
	}
}
