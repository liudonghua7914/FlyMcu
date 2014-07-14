#define  _IPC_EXCHANGE_GLOBAL_
#include "config.h"
#include "lpc17xx_lib.h"
#include "includes.h"
#include "UserType.h"
void ipcExchangeEvent(ULONG enumWhatEvent,ULONG lPara,BYTE *p,ULONG length)
{
	if(enumWhatEvent > EVENT_GLOBAL_INIT_MIN && enumWhatEvent < EVENT_GLOBAL_INIT_MAX)
	{
		ipcEventProcflySystem(enumWhatEvent,lPara,p,length);
		ipcEventProcflyFile(enumWhatEvent,lPara,p,length);
		//ipcEventProcFlylyEthernet(enumWhatEvent,lPara,p,length);
		ipcEventProcDemoDebug(enumWhatEvent,lPara,p,length);
		ipcEventProcFlyEEPROM(enumWhatEvent,lPara,p,length);
	}
	else if (enumWhatEvent > EVENT_GLOBAL_FLY_SYSTEM_MIN && enumWhatEvent < EVENT_GLOBAL_FLY_SYSTEM_MAX)
	{
		ipcEventProcflySystem(enumWhatEvent,lPara,p,length);
	}
	else if (enumWhatEvent > EVENT_GLOBAL_FLYETHERNET_MIN && enumWhatEvent < EVENT_GLOBAL_FLYETHERNET_MAX)
	{
		//ipcEventProcFlylyEthernet(enumWhatEvent,lPara,p,length);
	}
	else if (enumWhatEvent > EVENT_GLOBAL_FLY_FILE_MIN && enumWhatEvent < EVENT_GLOBAL_FLY_FILE_MAX)
	{
		ipcEventProcflyFile(enumWhatEvent,lPara,p,length);
	}
	else if (enumWhatEvent > EVENT_GLOBAL_DEMO_DEBUG_MIN && enumWhatEvent < EVENT_GLOBAL_DEMO_DEBUG_MAX)
	{
		ipcEventProcDemoDebug(enumWhatEvent,lPara,p,length);
	}
	else if (enumWhatEvent > EVENT_GLOBAL_FLY_EEPROM_MIN && enumWhatEvent < EVENT_GLOBAL_FLY_EEPROM_MAX)
	{
		ipcEventProcFlyEEPROM(enumWhatEvent,lPara,p,length);
	}
}

void ipcStartEvent(ULONG enumWhatEvent,ULONG lPara,BYTE *p,ULONG length)
{
	sbi(ipcExchangeInfo.EventSet[enumWhatEvent/32], enumWhatEvent%32);

	ipcExchangeEvent(enumWhatEvent,lPara,p,length);
}

void ipcClearEvent(UINT32 sourceEvent)
{
	cbi(ipcExchangeInfo.EventSet[sourceEvent/32], sourceEvent%32);
}

BOOL ipcWhatEventOn(UINT32 sourceEvent)
{
	return bit_is_set(ipcExchangeInfo.EventSet[sourceEvent/32], sourceEvent%32);
}

void ipcExchangeInit(void)
{
	memset(&ipcExchangeInfo,0,sizeof(IPC_EXCHANGE_INFO));
}