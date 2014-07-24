#define		_FLYEEPROMGOABLE_


#include "config.h"
#include "lpc17xx_lib.h"
#include "includes.h"
#include "UserType.h"


#define		_JMBuff(index)		flyeepromInfo.MessageBuf[index]
#define		_JMHx				flyeepromInfo.MessageBufHx
#define		_JMLx				flyeepromInfo.MessageBufLx
#define	 	_JMOFBIT			flyeepromInfo.MessageBufOverflow


#define 	BUFF_TO_ROLLBACK	0

#define		FOR_COMMUNICATION	1

/***************************************************************************************************************************
**函数名称:	 	WriteEEPROMFrameBuf
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
BOOL WriteEEPROMFrameBuf(BYTE Addr,BYTE *p,UINT Len)
{
	BYTE i = 0;
	BOOL bRes = FALSE;
	
	LIBMCU_DEBUG(EEPROM_DEBUG,("\r\n<<<<<WritEEPROMFrameBuf>>>>>: %x",Addr));
	for(i = 0;i < Len;i++)
	{
		LIBMCU_DEBUG(EEPROM_DEBUG,(" %x",p[i]));
	}
	OS_ENTER_CRITICAL();
	I2CStart();
	I2CWriteByte(Addr);
	bRes = WaitI2CACK();
	if(!bRes)
	{
		LIBMCU_DEBUG(EEPROM_DEBUG,("\r\n W->NACK 1"));
		return FALSE;
		
	}
	for(i = 0;i < Len;i++)
	{
		I2CWriteByte(p[i]);
		bRes = WaitI2CACK();
		if(!bRes)
		{
			LIBMCU_DEBUG(EEPROM_DEBUG,("\r\n W->NACK 2"));
			return FALSE;
			
		}
	}
	I2CStop();
	OS_EXIT_CRITICAL();
	return bRes;
}
/***************************************************************************************************************************
**函数名称:	 	ReadEEPROMFrameBuf
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
BOOL ReadEEPROMFrameBuf(BYTE ChipAddr,BYTE RegAddr,BYTE *p,UINT Len)
{
	BYTE i = 0;
	BOOL bRes = FALSE;
	UINT16 addr;
	
	for(i = 0;i < Len;i++)
	{	
		OS_ENTER_CRITICAL();
		I2CStart();
		I2CWriteByte(ChipAddr);
		bRes = WaitI2CACK();
		if(!bRes)
		{
			LIBMCU_DEBUG(EEPROM_DEBUG,("\r\n R->NACK 1"));
			return FALSE;
			
		}
	
		addr = RegAddr + i;
		I2CWriteByte(addr);
		bRes = WaitI2CACK();
		if(!bRes)
		{
			LIBMCU_DEBUG(EEPROM_DEBUG,("\r\n R->NACK 3"));
			return FALSE;
		}
		I2CStart();
		I2CWriteByte(ChipAddr | 0x01);
		bRes = WaitI2CACK();
		if(!bRes)
		{
			LIBMCU_DEBUG(EEPROM_DEBUG,("\r\n R->NACK 4"));
			return FALSE;
		}
		p[i] = I2CReadByte();
		I2CStop();
		OS_EXIT_CRITICAL();
	}	
	return bRes;
}	

/***************************************************************************************************************************
**函数名称:	 	messageDropMessage
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void messageDropMessage(void)
{	
	OS_ENTER_CRITICAL();
	if(BUFF_TO_ROLLBACK == _JMBuff(_JMLx))
	{
		_JMLx = 0;
	}
	else if(_JMBuff(_JMLx))
	{
		_JMLx = _JMLx + _JMBuff(_JMLx);
	}
	
	if(_JMLx >= MESSAGE_SIZE)
	{
		_JMLx = 0;
	}
	OS_EXIT_CRITICAL();
}
/***************************************************************************************************************************
**函数名称:	 	checkMessageHaveIdle
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
BOOL checkMessageHaveIdle(UINT Length)
{
	UINT iIdles;
	OS_ENTER_CRITICAL();
	if(Length > MESSAGE_SIZE - _JMHx - 1)
	{
		_JMBuff(_JMHx) = BUFF_TO_ROLLBACK;
		_JMHx = 0;
		if(0 == _JMLx)
		{
			messageDropMessage();
		}
	}
	
	if(_JMHx >= _JMLx)
	{
		iIdles = MESSAGE_SIZE - 1 -(_JMHx - _JMLx);
	}
	else
	{
		iIdles = _JMLx - _JMHx - 1;
	}
	OS_EXIT_CRITICAL();
	
	if(iIdles > Length)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
	
}
/***************************************************************************************************************************
**函数名称:	 	messageToFifo
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void messageToFifo(BYTE *p,UINT length)
{

	BYTE checkSum = 0;
	checkSum = length + 1;
	
	OS_ENTER_CRITICAL();
	if(length + 1 > 0XFF)
	{
		LIBMCU_DEBUG(EEPROM_DEBUG,("\r\n message error"));
		return;
	}
	
	
	while(!checkMessageHaveIdle(length + 1))
	{
		messageDropMessage();//扔掉最早的协议,腾出足够空间放新协议
 	}
	
	_JMBuff(_JMHx) = length + 1;
	_JMHx++;
	memcpy(&_JMBuff(_JMHx),p,length);
	_JMHx+=length;
	OS_EXIT_CRITICAL();
}
/***************************************************************************************************************************
**函数名称:	 	PutMessageToFrameBuff
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void PutMessageToFrameBuff(BYTE RegAddr,BYTE *p,UINT Len)
{
	BYTE i = 0;
	BYTE checkSum = Len + 1;
	OS_ENTER_CRITICAL();
	for(i = 0;i < Len;i++)
	{
		checkSum += p[i];
	}
	flyeepromInfo.sendBuf[0] = RegAddr;
	flyeepromInfo.sendBuf[1] = 0XFF;
	flyeepromInfo.sendBuf[2] = 0X55;
	flyeepromInfo.sendBuf[3] = Len + 1;
	memcpy(&flyeepromInfo.sendBuf[4],p,Len);
	flyeepromInfo.sendBuf[Len + 5] = checkSum;
	flyeepromInfo.sendBufLength = Len + 4;
	messageToFifo(flyeepromInfo.sendBuf,flyeepromInfo.sendBufLength);
	OS_EXIT_CRITICAL();
	OSSemPost(flyeepromInfo.pFlyEEPORMSemEvent);
}
/***************************************************************************************************************************
**函数名称:	 	getMessageFromBuff
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
UINT getMessageFromBuff(BYTE *p,UINT Len)
{
	UINT iReturnLength = 0;
	OS_ENTER_CRITICAL();
	if(_JMLx != _JMHx)
	{
		if (_JMBuff(_JMLx) == BUFF_TO_ROLLBACK)
		{
			_JMLx = 0;
		}
		else if(_JMBuff(_JMLx))
		{
			iReturnLength = _JMBuff(_JMLx)-1;
			if(iReturnLength < Len)
			{
				memcpy(p,&_JMBuff(_JMLx+1),iReturnLength);
				_JMLx = _JMLx + _JMBuff(_JMLx);
			}
			else
			{
				iReturnLength = 0;
			}
		}
	}
	OS_EXIT_CRITICAL();
	return iReturnLength;
}

/***************************************************************************************************************************
**函数名称:	 	getFlyEEPROMMessageBuf
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void getFlyEEPROMMessageBuf(void)
{
	UINT Length = 0;
	BYTE i;
	BOOL bRes = TRUE;
	Length = getMessageFromBuff(flyeepromInfo.FrameBuf,MESSAGE_SIZE);
	if(Length)
	{
		if(!WriteEEPROMFrameBuf(CHIP_ADDR,flyeepromInfo.FrameBuf,Length))
		{
			bRes = FALSE;
			LIBMCU_DEBUG(EEPROM_DEBUG,("\r\n WriteEEPROM Fail"));
		}
		
		if(bRes)
		{
			OSTimeDly(OS_TICKS_PER_SEC / 10);  //要延时一段时间才能去读....不然会出错
			if(ReadEEPROMFrameBuf(CHIP_ADDR,0X00,flyeepromInfo.recBuf,Length - 1))
			{
				for(i = 0;i < Length - 1;i++)
				{
					LIBMCU_DEBUG(EEPROM_DEBUG,("\r\n rec[%d] %x ",i,flyeepromInfo.recBuf[i]));
				}
			}
			else
			{
				LIBMCU_DEBUG(EEPROM_DEBUG,("\r\n ReadEEPROM Fail"));
			}
		}
		Length = 0;
	}
}
/***************************************************************************************************************************
**函数名称:	 	messageInit
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void messageInit(void)
{
	_JMHx = 0;
	_JMLx = 0;
}
/***************************************************************************************************************************
**函数名称:	 	ipcFlyEEPROMInit
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void ipcFlyEEPROMInit(void)
{
	flyeepromInfo.pFlyEEPORMSemEvent = NULL;
	FlyEEPROMTaskCreate();
	messageInit();
	flyeepromInfo.pFlyEEPORMSemEvent = OSSemCreate(0);
	if(NULL == flyeepromInfo.pFlyEEPORMSemEvent)
	{
		LIBMCU_DEBUG(EEPROM_DEBUG,("\r\n ipcFlyEEPROMInit Fail"));
		return;
	}
	LIBMCU_DEBUG(EEPROM_DEBUG,("\r\n ipcFlyEEPROMInit OK"));
}
/***************************************************************************************************************************
**函数名称:	 	ipcEventProcFlyEEPROM
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void ipcEventProcFlyEEPROM(ULONG enumWhatEvent,ULONG lPara,BYTE *p,uint8_t length)
{
	BOOL bRes = TRUE;
	switch(enumWhatEvent)
	{
		case EVENT_GLOBAL_MODULE_INIT:		ipcFlyEEPROMInit();				
											break;
		
		case EVENT_GLOBAL_FLY_EEPROM_CMD:	//PutMessageToFrameBuff(0X00,p,length);
																			
		default:							break;
	}
	
	if(bRes)
	{
		ipcClearEvent(enumWhatEvent);
	}
}
/***************************************************************************************************************************
**函数名称:	 	FlyEEPROMTask
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void FlyEEPROMTask(void *arg)
{
	INT8U Error;
	while(1)
	{
		OSSemPend(flyeepromInfo.pFlyEEPORMSemEvent,OS_TICKS_PER_SEC / 2,&Error);
		getFlyEEPROMMessageBuf(); 
	}
}
/***************************************************************************************************************************
**函数名称:	 	FlyEEPROMTaskCreate
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void FlyEEPROMTaskCreate(void)
{
	INT8U Res;
	Res = OSTaskCreate(FlyEEPROMTask, 								//执行函数
				 NULL,	  										//带入的参数
				 &GstkFlyEEPROM[FLYEEPORM_TASK_START_STK_SIZE-1],	//堆栈由高地址往底地址增长
				 (INT8U)PRIO_FLYEEPROM							//任务优先级
				 );
	if(OS_NO_ERR != Res)
	{
		LIBMCU_DEBUG(EEPROM_DEBUG,("\r\n Res = %d",Res));
	}
}
