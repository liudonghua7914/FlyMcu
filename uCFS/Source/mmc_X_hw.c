#include<stdio.h>
#include "interface.h"
#include "UserType.h"
#include "fs_include.h"
#include "mmc_x_hw.h"
#define	TIMESMAX  250
#define ID0		(0x00)

FS_u32 SDCardType = 0;
/*********************************************************************************************************************
**��������:	  FS_MMC_HW_X_BusyLedOff
**��������:
**��ڲ���:
**���ز���:
*********************************************************************************************************************/
void FS_MMC_HW_X_BusyLedOff(FS_u32 id)
{
	IO_Write(IO_LED0,FALSE);
}
/*********************************************************************************************************************
**��������:	  FS_MMC_HW_X_BusyLedOn
**��������:
**��ڲ���:
**���ز���:
*********************************************************************************************************************/
void FS_MMC_HW_X_BusyLedOn(FS_u32 id)
{
	IO_Write(IO_LED0,TRUE);
}
/*********************************************************************************************************************
**��������:	  FS_MMC_HW_X_PowerOn
**��������:
**��ڲ���:
**���ز���:
*********************************************************************************************************************/
void FS_MMC_HW_X_PowerOn(FS_u32 id)
{
	IO_Write(IO_SDCARDPWR,FALSE);	//�ϵ�
}
/*********************************************************************************************************************
**��������:	  FS_MMC_HW_X_PowerOff
**��������:
**��ڲ���:
**���ز���:
*********************************************************************************************************************/
void FS_MMC_HW_X_PowerOff(FS_u32 id)
{
	IO_Write(IO_SDCARDPWR,TRUE);	//�ϵ�
}
/*********************************************************************************************************************
**��������:	  FS_MMC_HW_X_BusyLedOn
**��������:
**��ڲ���:
**���ز���:
*********************************************************************************************************************/
void FS_MMC_HW_X_WriteCmd(FS_u32 id,FS_u8 cmd)
{
	
}
/*********************************************************************************************************************
**��������:	  FS_MMC_HW_X_ClockCard
**��������:
**��ڲ���:
**���ز���:
*********************************************************************************************************************/
void FS_MMC_HW_X_ClockCard(FS_u32 id, int num)
{
	FlySSP_Init(FLY_SSP0,num);	
}
/*********************************************************************************************************************
**��������:	  FS_MMC_HW_X_SetCS
**��������:	  SD��Ƭѡ�ź�
**��ڲ���:
**���ز���:
*********************************************************************************************************************/
void FS_MMC_HW_X_SetCS(FS_u32 id, char High)
{
	IO_Write(IO_SDCARDCS,(BOOL)High);
}
/*********************************************************************************************************************
**��������:	  FS_MMC_HW_X_AdjustFOP
**��������:
**��ڲ���:
**���ز���:
*********************************************************************************************************************/
FS_i8 FS_MMC_HW_X_AdjustFOP(FS_u32 id, FS_u32 tran_speed)
{
	return 0;
}
/*********************************************************************************************************************
**��������:	  FS_MMC_HW_X_CheckOCR
**��������:
**��ڲ���:
**���ز���:
*********************************************************************************************************************/
FS_i8 FS_MMC_HW_X_CheckOCR(FS_u32 id, FS_u32 ocr)
{
	return 0;
}
/*********************************************************************************************************************
**��������:	  FS_MMC_HW_X_GetFOP
**��������:
**��ڲ���:
**���ز���:
*********************************************************************************************************************/
FS_u32 FS_MMC_HW_X_GetFOP(FS_u32 id)
{
	return 0;
}
/*********************************************************************************************************************
**��������:	  FS_MMC_HW_X_CheckWP
**��������:
**��ڲ���:
**���ز���:
*********************************************************************************************************************/
FS_i8 FS_MMC_HW_X_CheckWP(FS_u32 id)
{
	return 0;
}
/*********************************************************************************************************************
**��������:	  FS_MMC_HW_X_DetectStatus
**��������:
**��ڲ���:
**���ز���:
*********************************************************************************************************************/
FS_i8 FS_MMC_HW_X_DetectStatus(FS_u32 id)
{
	FS_i8 Res = 0;
	Res = IO_Read(IO_SDCARDCD);
	return Res;
}
/*********************************************************************************************************************
**��������:	  FS_MMC_HW_X_WaitBusy
**��������:
**��ڲ���:
**���ز���:
*********************************************************************************************************************/
FS_u8 FS_MMC_HW_X_WaitBusy(FS_u32 id, FS_u32 maxwait) 
{
	return 0;
}
/*********************************************************************************************************************
**��������:	  FS_MMC_HW_X_ReadByte
**��������:
**��ڲ���:
**���ز���:
*********************************************************************************************************************/
FS_u8 FS_MMC_HW_X_ReadByte(FS_u32 id, FS_u32 maxwait)
{
	FS_u8 res;
	if(id != 0)
	{
		return 0;
	}
	FlySSP_ReadWrite(FLY_SSP0,FLYREAD,&res,1);
	return res;
}
/*********************************************************************************************************************
**��������:	  FS_MMC_HW_X_ReadByteNoSync
**��������:
**��ڲ���:
**���ز���:
*********************************************************************************************************************/
FS_u8 FS_MMC_HW_X_ReadByteNoSync(FS_u32 id)
{
   	FS_u8 res;
	FlySSP_ReadWrite(FLY_SSP0,FLYREAD,&res,1);
	return res;
}
/*********************************************************************************************************************
**��������:	  FS_MMC_HW_X_ReadBuf
**��������:
**��ڲ���:
**���ز���:
*********************************************************************************************************************/
void FS_MMC_HW_X_ReadBuf(FS_u32 id, FS_u8 *pdata,FS_u32 len)
{
	FS_u32 i;
	#if 0
	for(i = 0;i < len;i++)
	{
		pdata[i] = FlySSP_ReadByte(FLY_SSP0);//FS_MMC_HW_X_ReadByte(id,0);
	}
	#else
	FlySSP_ReadWrite(FLY_SSP0,FLYREAD,pdata,len);
	#endif
}
/*********************************************************************************************************************
**��������:	  FS_MMC_HW_X_ReadSingleBlock
**��������:
**��ڲ���:
**���ز���:
*********************************************************************************************************************/
FS_u8 FS_MMC_HW_X_ReadSingleBlock(FS_u32 id,FS_u8 *buf, int len, FS_u32 maxwait)
{
	FS_u8 Crc[2] = {0};
	FS_MMC_HW_X_ReadBuf(id,buf,len);
	Crc[0] = FS_MMC_HW_X_ReadByte(id,0);
	Crc[1] = FS_MMC_HW_X_ReadByte(id,0);
	return 0;
}
/*********************************************************************************************************************
**��������:	  FS_MMC_HW_X_WriteBytex
**��������:
**��ڲ���:
**���ز���:
*********************************************************************************************************************/
void FS_MMC_HW_X_WriteByte(FS_u32 id, FS_u8 data)
{
	if(id != 0)
	{
		return;
	}
	FlySSP_ReadWrite(FLY_SSP0,FLYWRITE,&data,1);
}
/*********************************************************************************************************************
**��������:	  FS_MMC_HW_X_WriteBuf
**��������:
**��ڲ���:
**���ز���:
*********************************************************************************************************************/
void FS_MMC_HW_X_WriteBuf(FS_u32 id, FS_u8 *pdata,FS_u32 len)
{
	FS_u32 i;
	for(i = 0;i < len;i++)
	{
		FS_MMC_HW_X_WriteByte(id,pdata[i]);
	}
}
/*****************************************************************************************************************************
**��������:	  FS__MMC_ReadSector
**��������:
**��ڲ���:
**���ز���:
******************************************************************************************************************************/
int FS__MMC_ReadSector(FS_u32 Unit,unsigned int Sector,unsigned char *pBuffer)
{
	BOOL bRes = FALSE;
	UINT count = 0;
	UINT i;
	BYTE Res = 0XFF;		
	
	BYTE SendBuf[] = {SD_CMD17,0X00,0X00,0X00,0X00,0x01};	
	
	if(eSDV2_0_SDHC != SDCardType)
	{
		Sector <<= 9;
//		LIBMCU_DEBUG("\r\n read offset Addr");
	}
	
//	LIBMCU_DEBUG("\r\n Read Addr %08x :",Sector);
	
	
	SendBuf[1] = (Sector >> 24) & 0XFF;
	SendBuf[2] = (Sector >> 16) & 0XFF;
	SendBuf[3] = (Sector >> 8) & 0XFF;
	SendBuf[4] = (Sector >> 0) & 0XFF;
	
	FS_MMC_HW_X_SetCS(Unit,0);
	FS_MMC_HW_X_WriteBuf(Unit,SendBuf,6);
	count = 0;
	do
	{
		Res = 0XFF;
		Res = FS_MMC_HW_X_ReadByte(Unit,0);
		if(0X00 == Res)
		{
			bRes = TRUE;
			break;
		}
		count++;
	}while(count < 200);
	if(!bRes)
	{
		LIBMCU_DEBUG(LIBMCU_DBG_ON,("\r\n R 1 %d %x ",bRes,Res));
	}
	if(bRes)
	{	
		bRes = FALSE;
		count = 0;
		do
		{
			Res = 0X00;
			Res = FS_MMC_HW_X_ReadByte(Unit,0);
			if(0XFE == Res)
			{
				bRes = TRUE;
				break;
			}
			count++;
		}while(count < 1000);
	}
	if(!bRes)
	{
		LIBMCU_DEBUG(MMC_DEBUG,("\r\n R 2 %d %x ",bRes,Res));
	}
	if(bRes)
	{
		if(NULL != pBuffer)
		{
			FS_MMC_HW_X_ReadSingleBlock(Unit,pBuffer,512,0);	//sizeof(SectorBuf)		
		}
		else
		{
			LIBMCU_DEBUG(MMC_DEBUG,("\r\n Read No place"));
		}
		
	}
	FS_MMC_HW_X_SetCS(Unit,1);
	FS_MMC_HW_X_WriteByte(Unit,0XFF);
	

	return 0;
}
/*********************************************************************************************************************
**��������:	  FS_MMC_HW_X_WriteSingleBlock
**��������:
**��ڲ���:
**���ز���:
*********************************************************************************************************************/
void FS_MMC_HW_X_WriteSingleBlock(FS_u32 id, FS_u8 *buf, int len)
{
	FS_MMC_HW_X_WriteBuf(id,buf,len);		//DATA
}
/*****************************************************************************************************************************
**��������:	  FS__MMC_WriteSector
**��������:
**��ڲ���:
**���ز���:
*******************************************************************************************************************************/
int FS__MMC_WriteSector(FS_u32 Unit,unsigned int Sector,unsigned char *pBuffer)
{
	BYTE SendBuf[] = {SD_CMD24,0X00,0X00,0X00,0X00,0xFF};	
	UINT count = 0;
	BYTE Res = 0XFF;
	BOOL bRes = FALSE;	
	if(eSDV2_0_SDHC != SDCardType)
	{
		Sector <<= 9;
//		LIBMCU_DEBUG("\r\n write offset Addr");
	}
	SendBuf[1] = (Sector >> 24) & 0XFF;
	SendBuf[2] = (Sector >> 16) & 0XFF;
	SendBuf[3] = (Sector >> 8) & 0XFF;
	SendBuf[4] = (Sector >> 0) & 0XFF;
	FS_MMC_HW_X_SetCS(Unit,0);
	FS_MMC_HW_X_WriteBuf(Unit,SendBuf,6);
	do
	{
		Res = 0XFF;
		Res = FS_MMC_HW_X_ReadByte(Unit,0);
		if(0X00 == Res)
		{
			bRes = TRUE;
			break;
		}
		count++;
	}while(count < 200);
	
	if(!bRes)
	{
		LIBMCU_DEBUG(MMC_DEBUG,("\r\n W  1 %d %x ",bRes,Res));
	}

	if(bRes)
	{
		count = 0;
		bRes = FALSE;	
		FS_MMC_HW_X_WriteByte(Unit,0XFE);					//0XFE
		FS_MMC_HW_X_WriteSingleBlock(Unit,pBuffer,512);		//data
		FS_MMC_HW_X_WriteByte(Unit,0XFF);					//CRC1
		FS_MMC_HW_X_WriteByte(Unit,0XFF);					//CRC2
		
		do
		{
			Res = 0XFF;
			Res = FS_MMC_HW_X_ReadByte(Unit,0);
			if(0X05 == (Res & 0X1F))
			{
				bRes = TRUE;
				break;
			}
			count++;		
		}while(count < 200);
		if(!bRes)
		{
			LIBMCU_DEBUG(MMC_DEBUG,("\r\n W  2 %d %x ",bRes,Res));
		}
		if(bRes)
		{
			do
			{
				count = 0;
				bRes = FALSE;	
				Res = 0XFF;
				Res = FS_MMC_HW_X_ReadByte(Unit,0);
				if(0X00 != Res)
				{
					bRes = TRUE;
					break;
				}
			}while(count < 200);
		}
		if(!bRes)
		{
			LIBMCU_DEBUG(MMC_DEBUG,("\r\n W  3 %d %x ",bRes,Res));
		}
	}
	
	FS_MMC_HW_X_SetCS(Unit,1);;	
	FS_MMC_HW_X_WriteByte(Unit,0xFF);
	return 0;
}





















/*----------------------------------------------------------------------------------------------------------------------------


													���³�ʼ����صĺ���....
													�Լ�д��.....


----------------------------------------------------------------------------------------------------------------------------*/

/*********************************************************************************************************************
**��������:	  FS_MMC_HW_X_ReadWrite
**��������:
**��ڲ���:
**���ز���:
*********************************************************************************************************************/
void FS_MMC_HW_X_ReadWrite(FS_u32 id,FS_u8 *pWrite, int wLen,FS_u8 *pRead, int rLen)
{
	FS_u8 times = 0;
	FS_u32 i;
	
	LIBMCU_DEBUG(MMC_DEBUG,("\r\n FS_MMC_HW_X_ReadWrite: "));
	for(i = 0;i < wLen;i++)
	{
		LIBMCU_DEBUG(MMC_DEBUG,("%x ",pWrite[i]));
	}
	FS_MMC_HW_X_SetCS(id,0);
	FlySSP_ReadWrite(FLY_SSP0,FLYWRITE,pWrite,wLen);
	do
	{
		FlySSP_ReadWrite(FLY_SSP0,FLYREAD,&pRead[0],1);
		if(0XFF != pRead[0])
		{
			break;
		}
		times++;
	}while(times < 200);
		
	if(rLen >= 2)
	{
		times = 0;
		do
		{
			FlySSP_ReadWrite(FLY_SSP0,FLYREAD,&pRead[1],1);
			if(0XFF != pRead[1])
			{
				break;
			}
			times++;
		}while(times < 200);
	}
	
	if(rLen > 2)
	{
		FlySSP_ReadWrite(FLY_SSP0,FLYREAD,&pRead[2],rLen - 2);
	}
	else
	
	if(rLen > 1)
	{
		FlySSP_ReadWrite(FLY_SSP0,FLYREAD,&pRead[1],rLen - 1);
	}	
	FS_MMC_HW_X_SetCS(id,1);
	FS_MMC_HW_X_WriteByte(id,0XFF);
}
/*****************************************************************************************************************************
**��������:	  FS_MMC_FS_MMC_HW_X_ResetCard
**��������:
**��ڲ���:
**���ز���:
*******************************************************************************************************************************/
BOOL FS_MMC_FS_MMC_HW_X_ResetCard(FS_u32 id)
{
	BOOL bRes = FALSE;
	BYTE SendBuf[] = {SD_CMD0,0X00,0X00,0X00,0X00,0x95};
	BYTE Res = 0XFF;
	FS_MMC_HW_X_ReadWrite(id,SendBuf,6,&Res,1);
	if(0X01 == Res)
	{
		bRes = TRUE;
	}
	return bRes;
}
/*****************************************************************************************************************************
**��������:	  FS_MMC_FS_MMC_HW_X_CheckType
**��������:
**��ڲ���:
**���ز���:
*******************************************************************************************************************************/
BOOL FS_MMC_FS_MMC_HW_X_CheckType(FS_u32 id)
{
	BYTE Res = 0XFF;
	BOOL bRes = FALSE;
	BYTE SendBuf[] = {SD_CMD55,0X00,0X00,0X00,0X00,0X01};
	UINT Count = 0;	
	do
	{	
		SendBuf[0] = SD_CMD55; 
		FS_MMC_HW_X_ReadWrite(id,SendBuf,6,&Res,1);	
		if(0XFF != Res)
		{
			Res = 0XFF;	
			SendBuf[0] = SD_CMD41;	
			SendBuf[1] = 0X40; 	
			FS_MMC_HW_X_ReadWrite(id,SendBuf,6,&Res,1);	
			if(0X00 == Res)
			{
				bRes = TRUE;
				break;
			}
		}
		Count++;
	}while(Count < 200);
	return bRes;
}
/*****************************************************************************************************************************
**��������:	  FS_MMC_FS_MMC_HW_X_ActiveCard
**��������:
**��ڲ���:
**���ز���:
*******************************************************************************************************************************/
BOOL FS_MMC_FS_MMC_HW_X_ActiveCard(FS_u32 id)
{
	BYTE SendBuf[] = {SD_CMD8,0X00,0X00,0X01,0XAA,0X87};
	BYTE RecBuf[5] = {0};
	FS_u16 Count;
	BOOL bRes = FALSE;
	BYTE Res = 0XFF;
	FS_MMC_HW_X_ReadWrite(id,SendBuf,6,RecBuf,5);	
	printf("\r\n RecBuf[0] = %x",RecBuf[0]);
	printf("\r\n RecBuf[1] = %x",RecBuf[1]);
	printf("\r\n RecBuf[2] = %x",RecBuf[2]);
	printf("\r\n RecBuf[3] = %x",RecBuf[3]);
	printf("\r\n RecBuf[4] = %x",RecBuf[4]);
	if(0X01 == RecBuf[0])
	{
		printf("\r\n SD V2.0 ");
		SDCardType = eSDV2_0;
		if(FS_MMC_FS_MMC_HW_X_CheckType(id))
		{
			bRes =  TRUE;
			
		}
	}
	else
	{
		printf("\r\n SD V1.0 ");
		SDCardType = eSDV1_0; 
		if(FS_MMC_FS_MMC_HW_X_CheckType(id))
		{	
			printf("\r\n SD Card ");
			bRes =  TRUE;
		}
		else
		{
			Count = 0;
			bRes = FALSE;
			do
			{
				FS_MMC_HW_X_ReadWrite(id,SendBuf,6,&Res,1);	
				if(0X00 == Res)
				{
					printf("\r\n MMC Card ");
					bRes = TRUE;
					SDCardType = eSDV1_0_MMC;
					break;
				}	
				Count++;
			}while(Count < 200);
		}
	}
	return bRes;
}

/*****************************************************************************************************************************
**��������:	  FS_MMC_FS_MMC_HW_X_ChcekOCR
**��������:
**��ڲ���:
**���ز���:
*******************************************************************************************************************************/
BOOL FS_MMC_FS_MMC_HW_X_ChcekOCR(FS_u32 id)
{
	BOOL bRes = FALSE;
	BYTE SendBuf[] = {SD_CMD58,0X00,0X00,0X00,0X00,0X00};
	BYTE RecBuf[5] = {0};
	FS_MMC_HW_X_ReadWrite(id,SendBuf,6,RecBuf,5);
	if(0X00 == RecBuf[0])
	{
		bRes = TRUE;
		printf("\r\n OCR[0] = %x",RecBuf[0]);
		printf("\r\n OCR[1] = %x",RecBuf[1]);
		printf("\r\n OCR[2] = %x",RecBuf[2]);
		printf("\r\n OCR[3] = %x",RecBuf[3]);
		printf("\r\n OCR[4] = %x",RecBuf[4]);
	}
	
	if(RecBuf[1] & 0X40)
	{
		SDCardType = eSDV2_0_SDHC;
		printf("\r\n SDHC ");
	}
	return bRes;
}
/*****************************************************************************************************************************
**��������:	  FS_MMC_FS_MMC_HW_X_SetCRC
**��������:
**��ڲ���:
**���ز���:
*******************************************************************************************************************************/
BOOL FS_MMC_FS_MMC_HW_X_SetCRC(FS_u32 id,BOOL bSet)
{
	BOOL bRes = FALSE;
	BYTE Res = 0XFF;
	BYTE SendBuf[] = {SD_CMD59,0X00,0X00,0X00,0X00,0X00};
	if(bSet)
	{
		FS_MMC_HW_X_ReadWrite(id,SendBuf,6,&Res,1);
		if(0X00 == Res)
		{
			bRes = TRUE;
		}
	}
	else
	{
		bRes = TRUE;
	}
	return bRes;
}
/*****************************************************************************************************************************
**��������:	  FS_MMC_FS_MMC_HW_X_SetBlockSize
**��������:
**��ڲ���:
**���ز���:
*******************************************************************************************************************************/
BOOL FS_MMC_FS_MMC_HW_X_SetBlockSize(FS_u32 id,FS_u32 Size)
{
	BYTE SendBuf[] = {SD_CMD16,0X00,0X00,0X00,0X00,0xFF};
	BYTE Res = 0XFF;
	BOOL bRes = FALSE;
	SendBuf[1] = (Size >> 24) & 0XFF;
	SendBuf[2] = (Size >> 16) & 0XFF;
	SendBuf[3] = (Size >> 8) & 0XFF;
	SendBuf[4] = (Size >> 0) & 0XFF;
	
	FS_MMC_HW_X_ReadWrite(id,SendBuf,6,&Res,1);
	if(!Res)
	{
		printf("\r\n Set Block Size OK");
		bRes = TRUE;
	}
	return bRes;
}
/*****************************************************************************************************************************
**��������:	  FS_MMC_FS_MMC_HW_X_ChcekCSD
**��������:
**��ڲ���:
**���ز���:
*******************************************************************************************************************************/
BOOL FS_MMC_FS_MMC_HW_X_ChcekCSD(FS_u32 id)
{
	BYTE i;
	BYTE SendBuf[] = {SD_CMD9,0X00,0X00,0X00,0X00,0xFF};
	BOOL bRes = FALSE;
	BYTE RecBuf[30] = {0}; 
	UINT len = 1 + 1 + 16 + 2; //0X00(1) + 0XFE(1) + data(16) + crc(2)
	FS_MMC_HW_X_ReadWrite(id,SendBuf,6,RecBuf,len);//ps: crc ֵ���Բ���,�������crc��������,��Ȼ����Ĳ����������
	if((0X00 == RecBuf[0]) && (0XFE == RecBuf[1]))
	{
		printf("\r\n CID: ");
		bRes = TRUE;
		for(i = 1;i <= len - 2;i++)
		{
			printf("%x ",RecBuf[i]);
		}
	}
	return bRes;
}
/*****************************************************************************************************************************
**��������:	  FS_MMC_FS_MMC_HW_X_ChcekCID
**��������:
**��ڲ���:
**���ز���:
*******************************************************************************************************************************/
BOOL FS_MMC_FS_MMC_HW_X_ChcekCID(FS_u32 id)
{
	BYTE i;
	BYTE SendBuf[] = {SD_CMD10,0X00,0X00,0X00,0X00,0xFF};
	BYTE RecBuf[30] = {0}; 
	BOOL bRes = FALSE;
	UINT len = 1 + 1 + 16 + 2; //0X00(1) + 0XFE(1) + data(16) + crc(2)
	FS_MMC_HW_X_ReadWrite(id,SendBuf,6,RecBuf,len);//ps: crc ֵ���Բ���,�������crc��������,��Ȼ����Ĳ����������
	if((0X00 == RecBuf[0]) && (0XFE == RecBuf[1]))
	{
		printf("\r\n CID: ");
		bRes = TRUE;
		for(i = 1;i <= len - 2;i++)
		{
			printf("%x ",RecBuf[i]);
		}
	}
	return bRes;
}
/*******************************************************************************************************************************
**��������:	  	FS_MMC_HW_X_Init
**��������:		1. �ϵ���ʱ74��CLK
				2. ��λ��(����CMD0 ָ��)
				3. ����SD�� ,�ڲ���ȡ��������(CMD1(����MMC��),CMD55,ACMD41)
				4. ��ѯOCR,��ѯ�������(CMD58)
				5. �����Ƿ�ʹ��CRC(CMD59)
				6. ���ö�д���ݿ鳤��(CMD16)
				7. ��ȡCID,��ȡ������Ϣ(CMD10)
				8. ��ȡCSD,��ȡ������Ϣ(CMD9)
**��ڲ���:
**���ز���:
********************************************************************************************************************************/
void FS_MMC_HW_X_Init(void)
{
   	FS_u32 cnt = 0;
	FS_u32 i = 0;
	FS_u32 Addr = 0;
	BOOL bRes = FALSE;
//	static FS_u8 buf[512];
	FS_MMC_HW_X_ClockCard(ID0,SSP_100KHz);
	
	/********************1. �ϵ���ʱ74��CLK********************/
	//1.1 �ϵ�
	FS_MMC_HW_X_PowerOn(ID0);
	FS_MMC_HW_X_SetCS(ID0,TRUE);
	//1.2 ��ʱ74��CLK
	for(cnt = 0;cnt < 10;cnt++)
	{
		FS_MMC_HW_X_WriteByte(ID0,0xFF);
	}
	/********************2. ��λ��(����CMD0 ָ��)**************/
	bRes = FS_MMC_FS_MMC_HW_X_ResetCard(ID0);
	if(FALSE == bRes)
	{
		FS_MMC_HW_X_PowerOff(ID0);
		printf("\r\n FS MMC Reset Fail");
	}
	/*********************3. ����SD�� **************************/
	if(bRes)
	{
		bRes = FS_MMC_FS_MMC_HW_X_ActiveCard(ID0);
		if(!bRes)
		{
			FS_MMC_HW_X_PowerOff(ID0);
			printf("\r\n FS MMC ACTIVE Fail");
		}
	}
	/***********************4. ��ѯOCR**************************/
	if(bRes)
	{
		bRes = FS_MMC_FS_MMC_HW_X_ChcekOCR(ID0);
		if(!bRes)
		{
			printf("\r\n FS MMC CHECK OCR Fail");
		}
	}
	/***********************5. �Ƿ�ʹ��CRC***********************/
	if(bRes)
	{
		bRes = FS_MMC_FS_MMC_HW_X_SetCRC(ID0,FALSE);
		if(!bRes)
		{
			printf("\r\n FS MMC CHECK SET CRC Fail");
		}
	}
	/**********************6. ���ö�д���ݿ鳤*******************/
	if(bRes)
	{
		bRes = FS_MMC_FS_MMC_HW_X_SetBlockSize(ID0,512);
		if(!bRes)
		{
			printf("\r\n FS MMC CHECK SET BLOCK Fail");
		}
	}
	/*********************7. ��ȡCID*****************************/
	if(bRes)
	{
		bRes = FS_MMC_FS_MMC_HW_X_ChcekCID(ID0);
		if(!bRes)
		{
			printf("\r\n FS MMC ChcekCID Fail");
		}
	}
	/*********************8. ��ȡCSD*****************************/
	if(bRes)
	{
		bRes = FS_MMC_FS_MMC_HW_X_ChcekCSD(ID0);
		if(!bRes)
		{
			printf("\r\n FS MMC ChcekCSD Fail");
		}
	}
	
	if(bRes)
	{
		printf("\r\n FS MMC Init OK");
		FS_MMC_HW_X_ClockCard(ID0,SSP_1MHz);
	}
}
//3b 0 6 1c dc dc f7 0 0 0 9 cf 3c 0 0 0 



















#if 0

for(cnt = 0;cnt < 512;cnt++)
{
	buf[cnt] = (cnt + 1) % 256;
}
FS__MMC_WriteSector(ID0,0X00,buf);
memset((void *)buf,0,sizeof(buf));
FS__MMC_ReadSector(ID0,0X2000,buf);
for(cnt = 0;cnt < 512;cnt++)
{
	if(!(cnt % 16))
	{
		printf("\r\n %08x :",cnt);
	}
	printf("%x ",buf[cnt]);
}
#endif

