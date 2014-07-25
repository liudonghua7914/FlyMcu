#ifndef 	___FLYFILE_H__
#define		___FLYFILE_H__

#include "def.h"
#include "includes.h"
#include "fs_include.h"
#define  		FLYFILE_TASK_START_STK_SIZE          200



#ifndef		_FLYFILEGOABLE_
	#define		FLYFILE_GLOBAL		extern
#else
	#define		FLYFILE_GLOBAL	

#endif
	

	
#define		BUF_SIZE 	(512)	
	



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
	BOOL bSDStatus;
	FS_FILE *fp;
	UINT FileLength;
	char recBuf[BUF_SIZE];
}FLYFILE_INFO;

FLYFILE_GLOBAL FLYFILE_INFO flyFileInfo;

FLYFILE_GLOBAL OS_STK  GstkflyFile[FLYFILE_TASK_START_STK_SIZE];

FLYFILE_GLOBAL void flyFileTaskCreate(void);
FLYFILE_GLOBAL void ipcEventProcflyFile(ULONG enumWhatEvent,ULONG lPara,BYTE *p,UINT length);



#endif
