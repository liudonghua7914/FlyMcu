#include "fs_conf.h"
#include "fs_port.h"
#include "fs_dev.h"
#include "fs_api.h"
#include "fs_fsl.h"
#include "fs_int.h"
#include "fs_os.h"
#include "fs_lbl.h"
#include "fs_fat.h"
#include "fs_clib.h"
#include "mmc_x_hw.h"


#include<stdio.h>




static FS_u32   _FS_mmc_logicalstart[FS_MMC_MAXUNIT];     /* start of partition */
static FS_u8     _FS_mmc_mbrbuffer[0x200];                 /* buffer for reading MBR */   
static char     _FS_mmc_diskchange[FS_MMC_MAXUNIT];       /* signal flag for driver */
static char     _FS_mmc_busycnt[FS_MMC_MAXUNIT];          /* counter for BSY LED on/off */

/***********************************************************************
**函数名称:	  FsDevStatus
**函数功能:
**入口参数:
**返回参数:
***********************************************************************/
static int FsDevStatus(FS_u32 Unit)
{
	FS_i8 a = 0;
	int x;
	int i = 0;
	static int init =0;
//	printf("\r\n FsDevStatus id: %d",Unit);
	
	if(!init)
	{
		for(init = 0;init < FS_MMC_MAXUNIT;init++)
		{
			_FS_mmc_diskchange[init] = 1;
		}
		init = 1;
	}
	
	
	if(Unit >= FS_MMC_MAXUNIT)
	{
		printf("\r\n NO MMC/SD Card Found!!!");
		return -1;
	}
	a = FS_MMC_HW_X_DetectStatus(Unit);
	if(a)
	{
		printf("\r\n No Card in Reader");
		return -1;
	}
	a = _FS_mmc_diskchange[Unit];
	if(a)
	{
		_FS_mmc_diskchange[Unit] = 0;
		FS_MMC_HW_X_Init();
		x =  FS__MMC_ReadSector(Unit, 0, (unsigned char*)&_FS_mmc_mbrbuffer[0]);
		if(x != 0)
		{
			printf("\r\n FS__MMC_ReadSector fail");
			return -1;
		}
		 /* Calculate start sector of the first partition */
		_FS_mmc_logicalstart[Unit]  = _FS_mmc_mbrbuffer[0x1c6];
		_FS_mmc_logicalstart[Unit] += (0x100UL * _FS_mmc_mbrbuffer[0x1c7]);
		_FS_mmc_logicalstart[Unit] += (0x10000UL * _FS_mmc_mbrbuffer[0x1c8]);
		_FS_mmc_logicalstart[Unit] += (0x1000000UL * _FS_mmc_mbrbuffer[0x1c9]);
		return FS_LBL_MEDIACHANGED;
	}
	return 0;
}
/***********************************************************************
**函数名称:	  FsDevRead
**函数功能:
**入口参数:
**返回参数:
***********************************************************************/
static int FsDevRead(FS_u32 Unit, FS_u32 Sector, void *buffer)
{
	int x;
//	printf("\r\n FsDevRead Unit: %d Sector: %d",Unit,Sector);
	if(Unit >= FS_MMC_MAXUNIT)
	{
		return -1;
	}
	x = FS__MMC_ReadSector(Unit, Sector + _FS_mmc_logicalstart[Unit], (unsigned char*)buffer);
	if (x != 0) 
	{
		x = -1;
	}
	return x;
}
/***********************************************************************
**函数名称:	  FsDevWrite
**函数功能:
**入口参数:
**返回参数:
***********************************************************************/
static int FsDevWrite(FS_u32 Unit, FS_u32 Sector, void *pBuffer)
{
	int x;
//	printf("\r\n FsDevWrite Unit: %d Sector: %d",Unit,Sector);

  
	if (Unit >= FS_MMC_MAXUNIT)
	{
		return -1;  /* No valid unit number */
	}
	x = FS__MMC_WriteSector(Unit, Sector + _FS_mmc_logicalstart[Unit], (unsigned char*)pBuffer);
	if (x != 0) 
	{
		x = -1;
	}
	return x;
}
/***********************************************************************
**函数名称:	  FsDevIoctl
**函数功能:
**入口参数:
**返回参数:
***********************************************************************/
static int FsDevIoctl(FS_u32 Unit, FS_i32 Cmd, FS_i32 Aux, void *pBuffer)
{
	 FS_u32 *info;
	int x;
	char a;
//	printf("\r\n FsDevIoctl Unit: %d Cmd: %x Aux: %x",Unit,Cmd,Aux);
	Aux = Aux;  /* Get rid of compiler warning */
	if (Unit >= FS_MMC_MAXUNIT) 
	{
		return -1;  /* No valid unit number */
	}
	switch (Cmd) 
	{
		case FS_CMD_INC_BUSYCNT:
			_FS_mmc_busycnt[Unit]++;
			if (_FS_mmc_busycnt[Unit] > 0) 
			{
				FS_MMC_HW_X_BusyLedOn(Unit);
			}
			break;
		case FS_CMD_DEC_BUSYCNT:
			_FS_mmc_busycnt[Unit]--;
			if (_FS_mmc_busycnt[Unit] <= 0) 
			{
				_FS_mmc_busycnt[Unit] = 0;
				FS_MMC_HW_X_BusyLedOff(Unit);
			}
			break;
		case FS_CMD_CHK_DSKCHANGE:
			a = FS_MMC_HW_X_DetectStatus(Unit);
			if (a) 
			{
				_FS_mmc_diskchange[Unit] = 1;
			}    
			break;
		case FS_CMD_GET_DEVINFO:
			if (!pBuffer)
			{
				return -1;
			}
			info = pBuffer;
			//FS__MMC_Init(Unit);
			x = FS__MMC_ReadSector(Unit, 0, (unsigned char*)&_FS_mmc_mbrbuffer[0]);
			if (x != 0) 
			{
				return -1;
			}
			/* hidden */
			*info = _FS_mmc_mbrbuffer[0x1c6];
			*info += (0x100UL * _FS_mmc_mbrbuffer[0x1c7]);
			*info += (0x10000UL * _FS_mmc_mbrbuffer[0x1c8]);
			*info += (0x1000000UL * _FS_mmc_mbrbuffer[0x1c9]);
			info++;
			/* head */
			*info = _FS_mmc_mbrbuffer[0x1c3]; 
			info++;
			/* sec per track */
			*info = _FS_mmc_mbrbuffer[0x1c4]; 
			info++;
			/* size */
			*info = _FS_mmc_mbrbuffer[0x1ca];
			*info += (0x100UL * _FS_mmc_mbrbuffer[0x1cb]);
			*info += (0x10000UL * _FS_mmc_mbrbuffer[0x1cc]);
			*info += (0x1000000UL * _FS_mmc_mbrbuffer[0x1cd]);
			break;
		default:
			break;
	}
	return 0;
}

const FS__device_type  FS__mmcdevice_driver = 
{
   "mmc",
   FsDevStatus,
   FsDevRead,
   FsDevWrite,
   FsDevIoctl
};
