#ifndef 	__USERTYPE_H__
#define		__USERTYPE_H__

#include "system_LPC17xx.h"

/**********************************/
#include<string.h>
#include<stdio.h>
#include<stdarg.h>
/**********************************/
#include "fs_port.h"
#include "fs_conf.h"
#include "fs_dev.h"
#include "fs_api.h"
/**********************************/
#include "def.h"
#include "ipcdef.h"
#include "ipcExchange.h"
#include "flySystemTask.h"
#include "demoDebugTask.h"
#include "flyFileTask.h"
#include "flyEthernetTask.h"
#include "flyeepromTask.h"
enum ePRIO
{
	PRIO_NON = 8,
	PRIO_FLYSYSTEM,
	PRIO_FLYFILE,
	PRIO_FLYEHTERNET,
	PRIO_FLYEEPROM,
	PRIO_DEMO
};

#endif
