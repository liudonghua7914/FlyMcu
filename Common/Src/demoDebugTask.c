#define  _DEMOGOABLE_
#include "config.h"
#include "lpc17xx_lib.h"
#include "includes.h"
#include "UserType.h"



#include "..\..\Common\Src\fifo.c"

T_Queue DemoQueue;
T_Queue *pDemoQueue = &DemoQueue;

/***************************************************************************************************************************
**函数名称:	 	printf_w
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void printf_w(const char *format, ...)
{
	int n = 0,m = 0,i = 0;
	UINT32 times;
	char *ptr;
	va_list args; 
	va_start(args, format);	
	
	times = OSTimeGet();
	m = snprintf(interfaceInfo.DebugTick,sizeof(interfaceInfo.DebugTick),"\r\n[%d.%03d]",times / 1000,times % 1000);
	
	
	n = vsprintf(interfaceInfo.DebugMsg,format,args);
	
	ptr = &interfaceInfo.DebugMsg[0];
	if(('\r' == ptr[0]) || ('\n' == ptr[0]))
	{
		ptr = &interfaceInfo.DebugMsg[1];
		n -= 1;
	}
	
	if(('\r' == ptr[1]) || ('\n' == ptr[1]))
	{
		ptr = &interfaceInfo.DebugMsg[2];
		n -= 1;
	}
	

	if(('\n' == ptr[n - 1]) || ('\r' == ptr[n - 1]))
	{
		n -= 1;
	}
	
	if(('\n' == ptr[n - 2]) || ('\r' == ptr[n - 2]))
	{
		n -= 1;
	}
	
	va_end(args);

	
	for(i = 0;i < m;i++)
	{
		#if 0
			UART_Send((LPC_UART_TypeDef *)DEBUG_PORT, (uint8_t *)&interfaceInfo.DebugTick[i], 1, BLOCKING);
		#else
			EnQueue(pDemoQueue,interfaceInfo.DebugTick[i]);
		#endif
	}
	
	for(i = 0;i < n;i++)
	{
		#if 0
			UART_Send((LPC_UART_TypeDef *)DEBUG_PORT, (uint8_t *)&ptr[i], 1, BLOCKING);
		#else
			EnQueue(pDemoQueue,ptr[i]);
		#endif
	}
	
	if(NULL != demoInfo.pDemoDebugEvent)
	{
		OSSemPost(demoInfo.pDemoDebugEvent);
	}
}

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
**函数名称:	 	DemofifoInit
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void DemofifoInit(void)
{
	QueueInit(pDemoQueue,demoInfo.DemoDebug,sizeof(demoInfo.DemoDebug));
	memset(&demoInfo,0,sizeof(demoInfo));
	demoInfo.pDemoDebugEvent = NULL;
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
	ResetUserTimer(&demoInfo.DemoDebugTickTime);
	demoInfo.pDemoDebugEvent = OSSemCreate(0);
	if(NULL == demoInfo.pDemoDebugEvent)
	{
		LIBMCU_DEBUG(DEMO_DEBUG,("pDemoDebugEvent OSSemCreate Fail"));
	}	
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
											LIBMCU_DEBUG(DEMO_DEBUG,("\r\n ipcDemoDebugInit OK"));
											break;
		
		
		default:							break;
	}
	
	if(bRes)
	{
		ipcClearEvent(enumWhatEvent);
	}
}
/***************************************************************************************************************************
**函数名称:	 	DemoDebugTick
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void DemoDebugTick(void)
{
	if(ReadUserTimer(&demoInfo.DemoDebugTickTime) > T_1S)
	{
		ResetUserTimer(&demoInfo.DemoDebugTickTime);
		LIBMCU_DEBUG(DEMO_DEBUG,("\r\n=============================================\r\n"));
		LIBMCU_DEBUG(DEMO_DEBUG,("\r\n IO_SDCARDCD = %d",IO_Read(IO_SDCARDCD)));
		LIBMCU_DEBUG(DEMO_DEBUG,("\r\n=============================================\r\n"));
		DemoEEPROM();
	}
}
/***************************************************************************************************************************
**函数名称:	 	DemoDebugMsg
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void DemoDebugMsg(void)
{
	UINT16 i;
	UINT16 len;
	DataType tmp;
	len = QueueEmpty(pDemoQueue);
	for(i = 0;i < len;i++)
	{
		tmp = DeQueue(pDemoQueue);
		UART_Send((LPC_UART_TypeDef *)DEBUG_PORT, (uint8_t *)&tmp, 1, BLOCKING);
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
	INT8U err;
	while(1)
	{
		if(NULL != demoInfo.pDemoDebugEvent)
		{
			OSSemPend(demoInfo.pDemoDebugEvent,500,&err);
		}	
		DemoDebugMsg();
		DemoDebugTick();
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


