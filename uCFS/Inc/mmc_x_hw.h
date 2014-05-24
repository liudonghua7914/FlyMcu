#ifndef 	__MMC_X_HW_H__
#define		__MMC_X_HW_H__
#include "fs_port.h"


enum eSDCARCMD
{
	SD_CMD0 = 0X00 | 0X40,
	SD_CMD1 = 0X01 | 0X40,
	SD_CMD2 = 0X02 | 0X40,
	SD_CMD3 = 0X03 | 0X40,
	SD_CMD4 = 0X04 | 0X40,
	SD_CMD5 = 0X05 | 0X40,
	SD_CMD6 = 0X06 | 0X40,
	SD_CMD7 = 0X07 | 0X40,
	SD_CMD8 = 0X08 | 0X40,
	SD_CMD9 = 0X09 | 0X40,
	SD_CMD10 = 0X0A | 0X40,
	SD_CMD16 = 0X10 | 0X40,
	SD_CMD17 = 0X11 | 0X40,
	SD_CMD24 = 0X18 | 0X40,
	SD_CMD39 = 0X27 | 0X40,
	SD_CMD41 = 0X29 | 0X40,
	SD_CMD55 = 0X37 | 0X40,
	SD_CMD58 = 0X3A | 0X40,
	SD_CMD59 = 0X3B | 0X40
};


enum eSDCARD
{
	eSDV1_0 = 0,
	eSDV1_0_MMC,
	eSDV2_0,
	eSDV2_0_SDHC
};

void FS_MMC_HW_X_BusyLedOff(FS_u32 id);
void FS_MMC_HW_X_BusyLedOn(FS_u32 id);
void FS_MMC_HW_X_ClockCard(FS_u32 id, int num);
void FS_MMC_HW_X_SetCS(FS_u32 id, char high);
FS_i8 FS_MMC_HW_X_AdjustFOP(FS_u32 id, FS_u32 tran_speed);
FS_i8 FS_MMC_HW_X_CheckOCR(FS_u32 id, FS_u32 ocr);
FS_u32 FS_MMC_HW_X_GetFOP(FS_u32 id);
FS_i8 FS_MMC_HW_X_CheckWP(FS_u32 id);
FS_i8 FS_MMC_HW_X_DetectStatus(FS_u32 id);
FS_u8 FS_MMC_HW_X_WaitBusy(FS_u32 id, FS_u32 maxwait);
FS_u8 FS_MMC_HW_X_ReadByte(FS_u32 id, FS_u32 maxwait);
FS_u8 FS_MMC_HW_X_ReadByteNoSync(FS_u32 id);
FS_u8 FS_MMC_HW_X_ReadSingleBlock(FS_u32 id,FS_u8 *buf, int len, FS_u32 maxwait);
void FS_MMC_HW_X_WriteByte(FS_u32 id, FS_u8 data);
void FS_MMC_HW_X_WriteSingleBlock(FS_u32 id, FS_u8 *buf, int len);
void FS_MMC_HW_X_Init(void);
int FS__MMC_ReadSector(FS_u32 Unit,unsigned int Sector,unsigned char *pBuffer);
int FS__MMC_WriteSector(FS_u32 Unit,unsigned int Sector,unsigned char *pBuffer);
#endif

