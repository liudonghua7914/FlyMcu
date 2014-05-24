#ifndef 	___FILESYSTEM_H__
#define		___FILESYSTEM_H__

#include "def.h"
#include "includes.h"
#include "fs_include.h"
#define  		FILESYSTE_TASK_START_STK_SIZE          600



#ifndef		_FILESYSTEMGOABLE_
	#define		FILESYSTEM_GLOBAL		extern
#else
	#define		FILESYSTEM_GLOBAL	

#endif
	

	
#define		SECTOR_SIZE 	(512)	
	
enum eSDStatus
{
	eSDInit = 1,
	eSDDelay,
	eSDResetCmd,
	eSDResetWait,
	eSDCheckType,
	eSDCheckOCR,
	eSDReadCID,
	eSDReadCSD,
	eSDSetBlock,
	eSDWrite,
	eSDRead,
	eSDError

};



	

typedef struct
{
	BYTE SDType;
	UINT32 SDCapacity;
	
}T_SDCARD_INFO;



typedef struct	
{
	BYTE *pWrite;
	UINT wLen;
	BYTE *pRrite;
	UINT rLen;
	BOOL bRelease;
	
}T_SSP_RW_INFO;

typedef struct
{
	BYTE xxx;
	FS_FILE *fp;
	UINT FileLength;
	
}FILE_SYSTEM_INFO;

FILESYSTEM_GLOBAL FILE_SYSTEM_INFO fileSystemInfo;

FILESYSTEM_GLOBAL OS_STK  GstkFileSYS[FILESYSTE_TASK_START_STK_SIZE];

FILESYSTEM_GLOBAL void FileSystemTaskCreate(void);
FILESYSTEM_GLOBAL void ipcEventProcFileSystem(ULONG enumWhatEvent,ULONG lPara,BYTE *p,uint8_t length);





















#endif
