#ifndef _IPC_DEF_H_
#define _IPC_DEF_H_
#include "def.h"

enum enumEventTrans{
EVENT_GLOBAL_TEST_ID = 0,
/*----------------------------------*/
	EVENT_GLOBAL_INIT_MIN,
	
	EVENT_GLOBAL_MODULE_INIT,
	
	EVENT_GLOBAL_INIT_MAX,
/*----------------------------------*/	
	EVENT_GLOBAL_FLY_SYSTEM_MIN,
	
	EVENT_GLOBAL_FLY_SYSTEM_MAX,	
/*----------------------------------*/	
	EVENT_GLOBAL_FLY_FILE_MIN,
	EVENT_GLOBAL_FLY_FILE_SDSTATUS,
	EVENT_GLOBAL_FLY_FILE_WRITE,
	EVENT_GLOBAL_FLY_FILE_READ,
	EVENT_GLOBAL_FLY_FILE_MAX,
/*----------------------------------*/	
	EVENT_GLOBAL_FLY_EEPROM_MIN,
	EVENT_GLOBAL_FLY_EEPROM_CMD,
	
	EVENT_GLOBAL_FLY_EEPROM_MAX,
/*----------------------------------*/	
	
	EVENT_GLOBAL_DEMO_DEBUG_MIN,

	EVENT_GLOBAL_DEMO_DEBUG_MAX
/*----------------------------------*/
};

#endif

