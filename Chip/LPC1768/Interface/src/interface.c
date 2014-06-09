#define  _INTERFACE_GLOBAL_
#include "config.h"
#include "includes.h"
#include "lpc17xx_lib.h"
#include "fs_include.h"
#include "UserType.h"






uint8_t menu1[] =
"\r\n******************************************\r\n"     
"              Hello World                 \r\n"
"             Created By: LDH		       \r\n"
"==============Flyaudio攻城狮==============\r\n"
"******************************************\r\n";


#define DIGIT(s, no) ((s)[no] - '0')

int hours = (10 * DIGIT(__TIME__, 0) + DIGIT(__TIME__, 1));
int minutes = (10 * DIGIT(__TIME__, 3) + DIGIT(__TIME__, 4));
int seconds = (10 * DIGIT(__TIME__, 6) + DIGIT(__TIME__, 7));
/* WARNING: This will fail in year 10000 and beyond, as it assumes
* that a year has four digits. */
int year = ( 1000 * DIGIT(__DATE__, 7)
+ 100 * DIGIT(__DATE__, 8)
+ 10 * DIGIT(__DATE__, 9)
+ DIGIT(__DATE__, 10));

/*
* Jan - 1
* Feb - 2
* Mar - 3
* Apr - 4
* May - 5
* Jun - 6
* Jul - 7
* Aug - 8
* Sep - 9
* Oct - 10
* Nov - 11
* Dec - 12
*/

/* Use the last letter as primary "key" and middle letter whenever
* two months end in the same letter. */
const int months = (__DATE__[2] == 'b' ? 2 :
(__DATE__[2] == 'y' ? 5 :
(__DATE__[2] == 'l' ? 7 :
(__DATE__[2] == 'g' ? 8 :
(__DATE__[2] == 'p' ? 9 :
(__DATE__[2] == 't' ? 10 :
(__DATE__[2] == 'v' ? 11 :
(__DATE__[2] == 'c' ? 12 :
(__DATE__[2] == 'n' ?
(__DATE__[1] == 'a' ? 1 : 6) :
/* Implicit "r" */
(__DATE__[1] == 'a' ? 3 : 4))))))))));
const int day = ( 10 * (__DATE__[4] == ' ' ? 0 : DIGIT(__DATE__, 4))+ DIGIT(__DATE__, 5));
typedef  void (*pFunction)(void);



#define	IO_SDA_INOUT(SdaInput)		(GPIO_SetDir(IIC_SDA_PORT,1 << IIC_SDA_PIN,SdaInput))
#define	IO_SCL_INOUT(SclInput)		(GPIO_SetDir(IIC_SCL_PORT,1 << IIC_SCL_PIN,SclInput))


#if 0

#define	IIC_SDA_H				IO_Write(IO_I2C_SDA,TRUE)	
#define	IIC_SDA_L				IO_Write(IO_I2C_SDA,FALSE)	

#define	IIC_SCL_H				IO_Write(IO_I2C_SCL,TRUE)		
#define	IIC_SCL_L				IO_Write(IO_I2C_SCL,FALSE)

#else

#define	IIC_SDA_H				LPC_GPIO0->FIOSET |= (0X01 << IIC_SDA_PIN)
#define	IIC_SDA_L				LPC_GPIO0->FIOCLR |= (0X01 << IIC_SDA_PIN)

#define	IIC_SCL_H				LPC_GPIO0->FIOSET |= (0X01 << IIC_SCL_PIN)
#define	IIC_SCL_L				LPC_GPIO0->FIOCLR |= (0X01 << IIC_SCL_PIN)

#endif
/***************************************************************************************************************************
**函数名称:	 	UserResetSystem
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void UserResetSystem(void)
{
	printf("\r\n****UserResetSystem***");
	NVIC_SystemReset();
}
/***************************************************************************************************************************
**函数名称:	 	GoT0WhatStatus
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void GoT0WhatStatus(uint32_t Address)
{
	pFunction Jump_To_Application;
	u32 JumpAddress;
	_DI();
	JumpAddress = *(volatile u32*)(Address + 4);	
	Jump_To_Application = (pFunction) JumpAddress;
	__set_PSP(*(volatile u32*) Address);
	__set_CONTROL(0);//设置使用主堆栈指针... 
	__set_MSP(*(volatile u32*) Address); 
	SCB->VTOR  = Address & 0x3FFFFF80;	
	Jump_To_Application();
}

/***************************************************************************************************************************
**函数名称:	 	I2C2_IRQHandler
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void SysTick_Handler(void)
{
 	
	SYSTICK_ClearCounterFlag();
	do 
	{
		interfaceInfo.SysTickCount++;
	} while (0 == interfaceInfo.SysTickCount);
	
	
	 #if OS_CRITICAL_METHOD == 3
        OS_CPU_SR cpu_sr;
    #endif 

    
    OS_ENTER_CRITICAL();                         
    OSIntNesting++;
    OS_EXIT_CRITICAL();

    OSTimeTick();                                                       /*  Call uC/OS-II's OSTimeTick()*/

    OSIntExit();              
}
/***************************************************************************************************************************
**函数名称:	 	ReadUserTimer
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
uint32_t ReadUserTimer(uint32_t *Timer)
{
	return (GetTickCount() - *Timer);
}
/***************************************************************************************************************************
**函数名称:	 	ReadUserTimer
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void ResetUserTimer(uint32_t *Timer)
{
	*Timer = GetTickCount();
}
/***************************************************************************************************************************
**函数名称:	 	SetUserTimer
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void SetUserTimer(uint32_t *timer,uint32_t T)
{
	*timer = GetTickCount() + T;
}
/***************************************************************************************************************************
**函数名称:	 	GetTickCount
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
uint32_t GetTickCount(void)
{
	return OSTimeGet();//interfaceInfo.SysTickCount;
}
/***************************************************************************************************************************
**函数名称:	 	Delayms
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void Delayms(uint32_t Time)
{
	uint32_t t;
	ResetUserTimer(&t);	
	while(ReadUserTimer(&t) < Time);
}

/***************************************************************************************************************************
**函数名称:	 	EINT3_IRQHandler
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void EINT0_IRQHandler(void)
{
	EXTI_ClearEXTIFlag(EXTI_EINT0);
}
/***************************************************************************************************************************
**函数名称:	 	EINT3_IRQHandler
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void EINT3_IRQHandler(void)
{
//	if(GPIO_GetIntStatus(ACC_IN_PORT, ACC_IN_PIN, 1))
//	{
//	  	GPIO_ClearInt(ACC_IN_PORT,(1<<ACC_IN_PIN));
//		//paraInfo->WakeUpSrc = 0X01;
//	}
//	if(GPIO_GetIntStatus(LCAN_RX_PORT, LCAN_RX_PIN, 1))
//	{
//	  	GPIO_ClearInt(LCAN_RX_PORT,(1<<LCAN_RX_PIN));
//		//paraInfo->WakeUpSrc = 0X02;
//	}

//	if(GPIO_GetIntStatus(WINCE_WP_PORT, WINCE_WP_PIN, 0))
//	{
//	  	GPIO_ClearInt(WINCE_WP_PORT,(1<<WINCE_WP_PIN));
//		//paraInfo->WakeUpSrc = 0X03;
//	}
}
/***************************************************************************************************************************
**函数名称:	 	I2C2_IRQHandler
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void I2C2_IRQHandler(void)
{
	interfaceInfo.IIcCount++;
	switch(LPC_I2C2->I2STAT & I2C_STAT_CODE_BITMASK)
	{
		case 0xF8:							
					LPC_I2C2->I2CONSET = I2C_I2CONSET_AA;
					LPC_I2C2->I2CONCLR = I2C_I2CONCLR_SIC;
					break;					
		
		case 0x60:
		case 0x70:			
					LPC_I2C2->I2CONSET = I2C_I2CONSET_AA;
					LPC_I2C2->I2CONCLR = I2C_I2CONCLR_SIC;
					break;

		case 0x80:	//接收数据
		case 0x90:							   
					//Temp = (uint8_t)LPC_I2C2->I2DAT;
					//FIFO_PUT_ONE_INT(NaviBoardCommRxFifo,Temp);	
					LPC_I2C2->I2CONSET = I2C_I2CONSET_AA;
					LPC_I2C2->I2CONCLR = I2C_I2CONCLR_SIC;
					break;
		case 0x88:	  
		case 0x98:
					LPC_I2C2->I2CONCLR = I2C_I2CONCLR_SIC;
					break;	

		case 0xA0:
					LPC_I2C2->I2CONCLR = I2C_I2CONCLR_SIC;
					break;

		case 0xA8:	
		case 0xB8:	//发送数据
//					if(FIFO_GET_COUNT(NaviBoardCommTxFifo))
//					{
//						FIFO_GET_ONE_INT(NaviBoardCommTxFifo,&Temp);
//						LPC_I2C2->I2DAT = Temp;	 				
//					}
//					else
//					{
//						LPC_I2C2->I2DAT = 0XAA;
//					}
					LPC_I2C2->I2CONSET = I2C_I2CONSET_AA;
					LPC_I2C2->I2CONCLR = I2C_I2CONCLR_SIC; 
					break;

		case 0xC0:	 
					LPC_I2C2->I2CONSET = I2C_I2CONSET_AA;
					LPC_I2C2->I2CONCLR = I2C_I2CONCLR_SIC;
					break;
		
		default:    
					printf("\r\n LPC_I2C2Status_Case=%x",(LPC_I2C2->I2STAT & I2C_STAT_CODE_BITMASK));
					LPC_I2C2->I2CONCLR = I2C_I2CONCLR_SIC | I2C_I2CONCLR_STAC;  	
					break;
	}
}
/***************************************************************************************************************************
**函数名称:	 	CANLPCRxMsg2RxStdMsg
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void CANLPCRxMsg2RxStdMsg(CAN_MSG_Type *lpc,CanRxMsg *Flystd)
{
	Flystd->DLC = lpc->len;
	if(lpc->format == STD_ID_FORMAT)
	{
	   Flystd->IDE = CAN_ID_STD;
	   Flystd->StdId = lpc->id;
	}
	else
	{
	   Flystd->IDE = CAN_ID_EXT;
	   Flystd->ExtId = lpc->id;
	}
	if(lpc->type == DATA_FRAME)
	{
	   Flystd->RTR = CAN_RTR_DATA;
	}
	else
	{
	   Flystd->RTR = CAN_RTR_REMOTE;
	}
 	memcpy(&Flystd->Data[0],&lpc->dataA[0],4);
	memcpy(&Flystd->Data[4],&lpc->dataB[0],4);
}
/***************************************************************************************************************************
**函数名称:	 	CANTxStdMsg2LPCTxMsg
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void CANTxStdMsg2LPCTxMsg(CanTxMsg *Flystd,CAN_MSG_Type *lpc)
{
	lpc->len = Flystd->DLC;							//字节数
	if(Flystd->IDE == CAN_ID_STD)
	{
		lpc->format = STD_ID_FORMAT;				//标准帧
		lpc->id = Flystd->StdId;
	}
	else
	{
		lpc->format = EXT_ID_FORMAT;
		lpc->id = Flystd->ExtId;
	}
	if(Flystd->RTR == CAN_RTR_DATA)
	{
		lpc->type = DATA_FRAME;						//数据帧
	}
	else
	{
		lpc->type = REMOTE_FRAME;
	}
	
	memcpy(&lpc->dataA[0],&Flystd->Data[0],4);
	memcpy(&lpc->dataB[0],&Flystd->Data[4],4);
}
/***************************************************************************************************************************
**函数名称:	 	CAN_IRQHandler
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void CAN_IRQHandler(void)
{
	uint8_t IntStatus;
	
	CanRxMsg RxData;
	CAN_MSG_Type LPC_RxCanMsg;
	
	
	IntStatus = CAN_IntGetStatus(LPC_CAN1);
	interfaceInfo.CanCount++;
	if((IntStatus >> 0) & 0x01)
	{
		CAN_ReceiveMsg(LPC_CAN1,&LPC_RxCanMsg);
		CANLPCRxMsg2RxStdMsg(&LPC_RxCanMsg,&RxData);
		//FIFO_PUT_ONE_INT(CarRxFifo,RxData);
	}
	

}
/***************************************************************************************************************************
**函数名称:	 	WDT_IRQHandler
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void WDT_IRQHandler(void)
{
	
}
/***************************************************************************************************************************
**函数名称:	 	UART1_IRQHandler
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void UART1_IRQHandler(void)
{
	uint32_t intsrc, tmp, tmp1;
	uint8_t Temp;
	intsrc = UART_GetIntId((LPC_UART_TypeDef*) LPC_UART1);
	tmp = intsrc & UART_IIR_INTID_MASK;
	if (tmp == UART_IIR_INTID_RLS)
	{
		tmp1 = UART_GetLineStatus((LPC_UART_TypeDef*)LPC_UART1);
		tmp1 &= (UART_LSR_OE | UART_LSR_PE | UART_LSR_FE \
				| UART_LSR_BI | UART_LSR_RXFE);
		if (tmp1) 
		{

		}
	}
	if ((tmp == UART_IIR_INTID_RDA) || (tmp == UART_IIR_INTID_CTI))
	{
		UART_Receive((LPC_UART_TypeDef *)LPC_UART1, &Temp, 1, NONE_BLOCKING);
		//FIFO_PUT_ONE_INT(UartRxFifo,Temp);	
		
	}
	interfaceInfo.UartCount++;
}
/***************************************************************************************************************************
**函数名称:	 	UART2_IRQHandler
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void UART2_IRQHandler(void)
{
	uint32_t intsrc, tmp, tmp1;
	uint8_t Temp;
	intsrc = UART_GetIntId((LPC_UART_TypeDef*) LPC_UART2);
	tmp = intsrc & UART_IIR_INTID_MASK;
	if (tmp == UART_IIR_INTID_RLS)
	{
		tmp1 = UART_GetLineStatus((LPC_UART_TypeDef*)LPC_UART2);
		tmp1 &= (UART_LSR_OE | UART_LSR_PE | UART_LSR_FE \
				| UART_LSR_BI | UART_LSR_RXFE);
		if (tmp1) 
		{
			
		}
	}
	if ((tmp == UART_IIR_INTID_RDA) || (tmp == UART_IIR_INTID_CTI))
	{
		UART_Receive((LPC_UART_TypeDef *)LPC_UART2, &Temp, 1, NONE_BLOCKING);
		//FIFO_PUT_ONE_INT(Uart2RxCarFifo,Temp);	
	}
}
/***************************************************************************************************************************
**函数名称:	 	UART3_IRQHandler
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void UART3_IRQHandler(void)
{
	uint32_t intsrc, tmp, tmp1;
	uint8_t Temp;
	intsrc = UART_GetIntId((LPC_UART_TypeDef*) LPC_UART3);
	tmp = intsrc & UART_IIR_INTID_MASK;
	if (tmp == UART_IIR_INTID_RLS)
	{
		tmp1 = UART_GetLineStatus((LPC_UART_TypeDef*)LPC_UART3);
		tmp1 &= (UART_LSR_OE | UART_LSR_PE | UART_LSR_FE \
				| UART_LSR_BI | UART_LSR_RXFE);
		if (tmp1) 
		{
			
		}
	}
	if ((tmp == UART_IIR_INTID_RDA) || (tmp == UART_IIR_INTID_CTI))
	{
		UART_Receive((LPC_UART_TypeDef *)LPC_UART3, &Temp, 1, NONE_BLOCKING);
		//FIFO_PUT_ONE_INT(Uart3RxCarFifo,Temp);	
	}
}
/***************************************************************************************************************************
**函数名称:	 	_DI
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void _DI(void)
{
	OS_ENTER_CRITICAL();
}
/***************************************************************************************************************************
**函数名称:	 	_EI
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void _EI(void)
{
	OS_EXIT_CRITICAL();
}
/***************************************************************************************************************************
**函数名称:	 	getBatteryVoltage
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
U8 getBatteryVoltage(void)
{
	return 0;
}
/***************************************************************************************************************************
**函数名称:	 	fputc
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
int fputc(int ch, FILE *f)
{
	#if 1
	uint8_t c = (uint8_t)ch;
	UART_Send((LPC_UART_TypeDef *)DEBUG_PORT, &c, 1, BLOCKING);
	#endif
	return (0);
}
/***************************************************************************************************************************
**函数名称:	 	I2CTickDelay
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void I2CTickDelay(void)
{
	//Delayus(TIME_2US);
//	Delayms(T_1MS);
	uint32_t Count = 0;
	for(Count = 0;Count < 10000;Count++)
	{
		;
	}
}
/***************************************************************************************************************************
**函数名称:	 	I2CStart
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void I2CStart(void)
{
	IO_SDA_INOUT(1);
	IO_SCL_INOUT(1);
	IIC_SDA_H;
	IIC_SCL_H;
	I2CTickDelay();
	IIC_SDA_L;
	I2CTickDelay();
}
/***************************************************************************************************************************
**函数名称:	 	I2CStop
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void I2CStop(void)
{
	IIC_SCL_L;
	IIC_SDA_L;
	I2CTickDelay();
	IIC_SCL_H;
	I2CTickDelay();
	IIC_SDA_H;
	I2CTickDelay();
}
/***************************************************************************************************************************
**函数名称:	 	I2CACK
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void I2cACK(void)
{
	
}
/***************************************************************************************************************************
**函数名称:	 	WaitI2CACK
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
BOOL WaitI2CACK(void)
{
	BOOL bRes = FALSE;
	UINT count = 0;
	IIC_SCL_L;
	I2CTickDelay();
	IIC_SCL_H;
 	IO_SDA_INOUT(0);
	do
	{  
		if(!IO_Read(IO_I2C_SDA)) 
		{
			bRes = TRUE;
			break;
		}		
		count++;
	}while(count < 300);//count--
	I2CTickDelay();
	IIC_SCL_L;
	I2CTickDelay();
	IO_SDA_INOUT(1);
	if(!bRes)
	{
		printf("\r\n Stop");
		I2CStop();
	}
	return bRes;
}
/***************************************************************************************************************************
**函数名称:	 	I2CReadByte
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
BYTE I2CReadByte(void)
{
	BYTE i,Dat;  
	IO_SDA_INOUT(0);
	Dat=0;  
	for(i=0;i<8;i++)  
	{  
		IIC_SCL_L;
		I2CTickDelay();
		IIC_SCL_H;	
		Dat<<=1;  
		if(IO_Read(IO_I2C_SDA))
		{  
			Dat|=0x01;   
		}     
		I2CTickDelay();
	}
	IO_SDA_INOUT(1);
	return Dat;
}
/***************************************************************************************************************************
**函数名称:	 	I2CWriteByte
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void I2CWriteByte(BYTE data)
{
	u8 i;  
	for(i = 0;i < 8;i++)
	{
		IIC_SCL_L;
		if(data&0x80)  
		{  
			IIC_SDA_H;
		}  
		else  
		{  
			IIC_SDA_L;
		}   
		data<<=1;  
		I2CTickDelay();
			
		IIC_SCL_H;
		I2CTickDelay();
	}
}
/***************************************************************************************************************************
**函数名称:	 	PrintWellcomeMsg
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void PrintWellcomeMsg(void)
{
	printf("%s",menu1);
	printf("Build in:");
	printf("%d-",year);
	printf("%d-",months);
	printf("%d ",day);
	printf("%d:",hours);
	printf("%d:",minutes);
	printf("%d",seconds);
}
/***************************************************************************************************************************
**函数名称:	 	SysTickInit
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void SysTickInit(void)
{
//	SysTick_Config((100000000UL / OS_TICKS_PER_SEC)); //10ms
	SYSTICK_InternalInit(1);
	SYSTICK_IntCmd(ENABLE);
	SYSTICK_Cmd(ENABLE);
}
/***************************************************************************************************************************
**函数名称:	 	SysTickDeInit
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void SysTickDeInit(void)
{
   	SYSTICK_IntCmd(DISABLE);
	SYSTICK_Cmd(DISABLE);
}
/***************************************************************************************************************************
**函数名称:	 	DebugInit
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void DebugInit(void)
{
	UART_CFG_Type UARTConfigStruct;
	PINSEL_CFG_Type PinCfg;	
	
	PinCfg.Funcnum = FUNCNUM;		   
	PinCfg.OpenDrain = 0;			  
	PinCfg.Pinmode = 0;
	PinCfg.Portnum = PORTNUM;	
	PinCfg.Pinnum = PINNUM1;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = PINNUM2;
	PINSEL_ConfigPin(&PinCfg);

	UART_ConfigStructInit(&UARTConfigStruct);
	UARTConfigStruct.Baud_rate = 115200;
	UART_Init((LPC_UART_TypeDef *)DEBUG_PORT, &UARTConfigStruct);
	UART_TxCmd((LPC_UART_TypeDef *)DEBUG_PORT, ENABLE);
}
/***************************************************************************************************************************
**函数名称:	 	DebugDeInit
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void DebugDeInit(void)
{
	UART_DeInit((LPC_UART_TypeDef *)DEBUG_PORT);
}

/***************************************************************************************************************************
**函数名称:	 	FlyUART_TransmitToCar
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void FlyUART_TransmitToCar(uint8_t Num,uint8_t *pdata,uint8_t len)
{
#if 1
	uint8_t i;
	LPC_UART_TypeDef *Uartx;
	if(3 == Num)
	{
		Uartx = LPC_UART3;
	}
	else if(2 == Num)
	{
		Uartx = LPC_UART2;
	}
	for(i = 0;i < len;i++)
	{
		UART_Send((LPC_UART_TypeDef *)Uartx,&pdata[i],1,BLOCKING);
	}
#endif
}
/***************************************************************************************************************************
**函数名称:	 	AdcsInit
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void AdcsInit(void)
{
	PINSEL_CFG_Type PinCfg;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 2;

	PinCfg.Funcnum = 1;
	PinCfg.Pinnum = 25;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);
 	PinCfg.Pinnum = 26;
	PINSEL_ConfigPin(&PinCfg);

	PinCfg.Portnum = 0;
	PinCfg.Funcnum = 2;
 	PinCfg.Pinnum = 2;
	PINSEL_ConfigPin(&PinCfg);
 	PinCfg.Pinnum = 3;
	PINSEL_ConfigPin(&PinCfg);


	PinCfg.Funcnum = 3;
	PinCfg.Pinnum = 30;
	PinCfg.Portnum = 1;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 31;
	PinCfg.Portnum = 1;
	PINSEL_ConfigPin(&PinCfg);
	ADC_Init(LPC_ADC, 200000);
	ADC_ChannelCmd(LPC_ADC,ADC_CHANNEL_0,ENABLE);
	ADC_ChannelCmd(LPC_ADC,ADC_CHANNEL_1,ENABLE);
	ADC_ChannelCmd(LPC_ADC,ADC_CHANNEL_2,ENABLE);
	ADC_ChannelCmd(LPC_ADC,ADC_CHANNEL_3,ENABLE);
	ADC_ChannelCmd(LPC_ADC,ADC_CHANNEL_4,ENABLE);
	ADC_ChannelCmd(LPC_ADC,ADC_CHANNEL_5,ENABLE);
   	//Start burst conversion
	ADC_BurstCmd(LPC_ADC,ENABLE);
}
/***************************************************************************************************************************
**函数名称:	 	GetAdcChannelValue
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
uint16_t GetAdcChannelValue(uint8_t Channel)
{
	while(SET != ADC_ChannelGetStatus(LPC_ADC,Channel,1));
	return (ADC_ChannelGetData(LPC_ADC,Channel));
}
/***************************************************************************************************************************
**函数名称:	 	AdcsDeInit
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void AdcsDeInit(void)
{
   ADC_DeInit(LPC_ADC);
}
/***************************************************************************************************************************
**函数名称:	 	naviBoardCommInit
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void naviBoardCommInit(void)
{
	uint32_t temp = 0;
	PINSEL_CFG_Type PinCfg;
	PinCfg.OpenDrain = 1;
	PinCfg.Pinmode = 0;
	PinCfg.Funcnum = 2;
	PinCfg.Pinnum = 10;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 11;
	PINSEL_ConfigPin(&PinCfg);

	PinCfg.Funcnum = 0;
	PinCfg.Pinnum = 15;
	PinCfg.Portnum = 1;
	PINSEL_ConfigPin(&PinCfg);

	// Initialize Slave I2C peripheral
	LPC_SC->PCONP |= CLKPWR_PCONP_PCI2C2 & CLKPWR_PCONP_BITMASK;
	LPC_SC->PCLKSEL1 &= ~(CLKPWR_PCLKSEL_BITMASK(CLKPWR_PCLKSEL_I2C2 - 32));
	LPC_SC->PCLKSEL1 |= (CLKPWR_PCLKSEL_SET(CLKPWR_PCLKSEL_I2C2 - 32, CLKPWR_PCLKSEL_CCLK_DIV_2));

	//I2C_SetClock(I2CDEV_S, baudrate);
	temp = ( (SystemCoreClock / CLKPWR_PCLKSEL_CCLK_DIV_2) / 100000);
	LPC_I2C2->I2SCLH = (uint32_t)(temp / 2);
	LPC_I2C2->I2SCLL = (uint32_t)(temp - LPC_I2C2->I2SCLH);

    LPC_I2C2->I2CONCLR = (I2C_I2CONCLR_AAC | I2C_I2CONCLR_STAC | I2C_I2CONCLR_I2ENC);

	/* Set  Own slave address for I2C device */
	LPC_I2C2->I2ADR0 = (((uint32_t)(0XA0 << 1))|(0x01))& I2C_I2ADR_BITMASK;
	LPC_I2C2->I2MASK0 = I2C_I2MASK_MASK((uint32_t)(0XFF));

    /* Disable I2C2 interrupt */
    NVIC_DisableIRQ(I2C2_IRQn);
    NVIC_SetPriority(I2C2_IRQn, I2C2_IRQn);
	LPC_I2C2->I2CONSET = I2C_I2CONSET_I2EN;
	LPC_I2C2->I2CONSET = I2C_I2CONSET_AA;
	LPC_I2C2->I2CONCLR = I2C_I2CONCLR_SIC | I2C_I2CONCLR_STAC;
	I2C_IntCmd(LPC_I2C2, TRUE);
}
/***************************************************************************************************************************
**函数名称:	 	WatchdogInit
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void WatchdogInit(void)
{
#if(WTD_EN == 1)
	NVIC_SetPriority(WDT_IRQn, 0x10);
	WDT_Init(WDT_CLKSRC_IRC, WDT_MODE_RESET);
	NVIC_EnableIRQ(WDT_IRQn);
	WDT_Start(WDT_TIMEOUT);
#endif	
}
/***************************************************************************************************************************
**函数名称:	 	WdtForWakeUp
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void WdtForWakeUp(uint32_t Time)
{
	WDT_Init(WDT_CLKSRC_IRC, WDT_MODE_INT_ONLY);
	WDT_Start(WDT_TIMEOUT);
}
/***************************************************************************************************************************
**函数名称:	 	naviBoardCommDeInit
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void naviBoardCommDeInit(void)
{
	I2C_DeInit(LPC_I2C2);
	NVIC_DisableIRQ(I2C2_IRQn);
}

/**********************************************************************************************************************
**函数名称:	 	IntoDeepSleep 
**函数功能:
**入口参数:
**返回参数:
**********************************************************************************************************************/
void IntoDeepSleep(void)
{	
	EXTI_InitTypeDef EXTI_InitStruct;
	printf("\r\n IntoDeepSleep");
	EXTI_Init();	
	EXTI_InitStruct.EXTI_Line = EXTI_EINT3;
	EXTI_InitStruct.EXTI_Mode = EXTI_MODE_EDGE_SENSITIVE;
	EXTI_InitStruct.EXTI_polarity =EXTI_POLARITY_LOW_ACTIVE_OR_FALLING_EDGE; 	
	EXTI_Config(&EXTI_InitStruct);
//	LPC_GPIOINT->IO0IntEnF =  (1<<ACC_IN_PIN)|(1<<LCAN_RX_PIN);	//设置ACC_IN_PIN，LCAN_RX_PIN为下降沿中断

//	LPC_GPIOINT->IO2IntEnR =  (1<<WINCE_WP_PIN);
	
	NVIC_EnableIRQ(EINT3_IRQn);
	#if(WTD_EN == 1)
		WDT_Start(WDT_1HH);	// carBasicInfo.WdtResetPreTime
	#endif
#if 1
	LPC_SC->PLL0CON &= ~(1<<1); /* Disconnect the main PLL (PLL0) */
	LPC_SC->PLL0FEED = 0xAA; /* Feed */
	LPC_SC->PLL0FEED = 0x55; /* Feed */
	while ((LPC_SC->PLL0STAT & (1<<25)) != 0x00); /* Wait for main PLL (PLL0) to disconnect */
	LPC_SC->PLL0CON &= ~(1<<0); /* Turn off the main PLL (PLL0) */
	LPC_SC->PLL0FEED = 0xAA; /* Feed */
	LPC_SC->PLL0FEED = 0x55; /* Feed */
	while ((LPC_SC->PLL0STAT & (1<<24)) != 0x00); /* Wait for main PLL (PLL0) to shut down */
	CLKPWR_DeepSleep();
#else
	CLKPWR_Sleep();
#endif
	NVIC_DisableIRQ(EINT3_IRQn);
	//GoT0WhatStatus(UserProgramAddressEntry);	//
}
/***************************************************************************************************************************
**函数名称:	 	IO_Read
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
BOOL IO_Read(U32 IOEnum)
{
   	uint8_t Pin,Port;
	BOOL bRes = TRUE;
	switch(IOEnum)
	{						
		case IO_LED0:			Port = IO_LED0_PORT;
								Pin = IO_LED0_PIN;
								break;
		case IO_LED1:			Port = IO_LED1_PORT;
								Pin = IO_LED1_PIN;
								break;
		case IO_LED2:			Port = IO_LED2_PORT;
								Pin = IO_LED2_PIN;
								break;
		case IO_LED3:			Port = IO_LED3_PORT;
								Pin = IO_LED3_PIN;
								break;
		case IO_LED4:			Port = IO_LED4_PORT;
								Pin = IO_LED4_PIN;
								break;
		case IO_LED5:			Port = IO_LED5_PORT;
								Pin = IO_LED5_PIN;
								break;
		case IO_LED6:			Port = IO_LED6_PORT;
								Pin = IO_LED6_PIN;
								break;
		case IO_LED7:			Port = IO_LED7_PORT;
								Pin = IO_LED7_PIN;
								break;
					
		
		case IO_SDCARDCD:		Port = IO_SDCD_PORT;
								Pin = IO_SDCD_PIN;
								break;

		case IO_SDCARDPWR:		Port = IO_SDPWR_PORT;
								Pin = IO_SDPWR_PIN;
								break;
								
		case IO_I2C_SDA:		Port = IIC_SDA_PORT;
								Pin = IIC_SDA_PIN;
								break;
		
		case IO_I2C_SCL:		Port = IIC_SCL_PORT;
								Pin = IIC_SCL_PIN;
								break;
								
		default:				printf("\r\n Read Other Port");			
								bRes = FALSE;
								break;
	}
	if(bRes)
	{
		bRes = (BOOL)((GPIO_ReadValue(Port) >> Pin) & 0X01);
	}
	return bRes;
}
/***************************************************************************************************************************
**函数名称:	 	IO_Write
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void IO_Write(U32 IOEnum,BOOL status)
{
	uint8_t Pin,Port;
	BOOL bRes = TRUE;
	
	switch(IOEnum)
	{
		case IO_LED0:			Port = IO_LED0_PORT;
								Pin = IO_LED0_PIN;
								break;
		case IO_LED1:			Port = IO_LED1_PORT;
								Pin = IO_LED1_PIN;
								break;
		case IO_LED2:			Port = IO_LED2_PORT;
								Pin = IO_LED2_PIN;
								break;
		case IO_LED3:			Port = IO_LED3_PORT;
								Pin = IO_LED3_PIN;
								break;
		case IO_LED4:			Port = IO_LED4_PORT;
								Pin = IO_LED4_PIN;
								break;
		case IO_LED5:			Port = IO_LED5_PORT;
								Pin = IO_LED5_PIN;
								break;
		case IO_LED6:			Port = IO_LED6_PORT;
								Pin = IO_LED6_PIN;
								break;
								
		case IO_LED7:			Port = IO_LED7_PORT;
								Pin = IO_LED7_PIN;
								break;

		case IO_SDCARDCS:		Port = IO_SDCS_PORT;
								Pin = IO_SDCS_PIN;
								break;	
			
		
		case IO_SDCARDCD:		Port = IO_SDCD_PORT;
								Pin = IO_SDCD_PIN;
								break;

		case IO_SDCARDPWR:		Port = IO_SDPWR_PORT;
								Pin = IO_SDPWR_PIN;
								break;

		case IO_I2C_SCL:		Port = IIC_SCL_PORT;
								Pin = IIC_SCL_PIN;
								break;
		
		case IO_I2C_SDA:		Port = IIC_SDA_PORT;
								Pin = IIC_SDA_PIN;
								break;
		
		default:				printf("\r\n Other Port");
								bRes = FALSE;
								break;
	}
	
	if(bRes)
	{
		GPIO_SetDir(Port,1 << Pin,1);
		if(status)
		{
			GPIO_SetValue(Port,1 << Pin);
		}
		else
		{
			GPIO_ClearValue(Port,1 << Pin);
		}
	}
}
/***************************************************************************************************************************
**函数名称:	 	IO_WriteExtended
**函数功能:	 	写扩展IO
**入口参数:
**返回参数:
***************************************************************************************************************************/
void IO_WriteExtended(uint8_t Port,uint8_t Pin,uint8_t status)
{
	if(status)
	{
		GPIO_SetValue(Port,1 << Pin);
	}
	else
	{
		GPIO_ClearValue(Port,1 << Pin);
	}
}
/***************************************************************************************************************************
**函数名称:	 	FlyUART_Band
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void FlyUART_Band(U32 ComEnum,U32 baudrate)
{
	
	
}
/***************************************************************************************************************************
**函数名称:	 	FlyUART_Init
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void FlyUART_Init(U32 ComEnum,U32 baudrate)
{
	LPC_UART_TypeDef *Uartx;
	IRQn_Type IRQx;
	UART_CFG_Type UARTConfigStruct;
	UART_FIFO_CFG_Type UARTFIFOConfigStruct;
	PINSEL_CFG_Type PinCfg;
	PinCfg.Funcnum = 2;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Portnum = 2;
	PinCfg.Pinnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 1;
	PINSEL_ConfigPin(&PinCfg);

	printf("\r\nComEnum = %d baudrate = %d",ComEnum,baudrate);
	switch(ComEnum)
	{
		case 0X01:		break;
		case 0X02:		break;
		case 0X03:		
		case 0X04:		
		case 0X05:		Uartx = (LPC_UART_TypeDef *)LPC_UART1;
						IRQx = UART1_IRQn;
						printf("\r\n LPC UART");
						break;
			
	}
	UARTConfigStruct.Baud_rate = baudrate;
	UART_ConfigStructInit(&UARTConfigStruct);
	UART_FIFOConfigStructInit(&UARTFIFOConfigStruct);
	UART_Init(Uartx, &UARTConfigStruct);
	UART_FIFOConfig(Uartx, &UARTFIFOConfigStruct);
	UART_TxCmd(Uartx, ENABLE);
	UART_IntConfig(Uartx, UART_INTCFG_RBR, ENABLE);
	NVIC_SetPriority(IRQx, IRQx);	
    NVIC_EnableIRQ(IRQx);
}
/***************************************************************************************************************************
**函数名称:	 	FlyUART_DeInit
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void FlyUART_DeInit(U32 ComEnum)
{
	UART_DeInit((LPC_UART_TypeDef *)LPC_UART1);
	NVIC_DisableIRQ(UART1_IRQn);	
}
/***************************************************************************************************************************
**函数名称:	 	FlyUART_Transmit
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void FlyUART_Transmit(U32 ComEnum,uint8_t pdata)
{
	switch(ComEnum)
	{
		case 0X01:		break;
		case 0X02:		break;
		case 0X03:		
		case 0X04:		
		case 0X05:		
						UART_Send((LPC_UART_TypeDef *)LPC_UART1,&pdata,1,BLOCKING);
						break;
	}
}

/***************************************************************************************************************************
**函数名称:	 	FlyCAN_Init
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void FlyCAN_Init(U32 CanEnum,U32 baudrate,U32 *pIDFilterList)
{
   	PINSEL_CFG_Type PinCfg;
	PinCfg.Funcnum = 1;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	if(1 == CanEnum)
   	{
   		PinCfg.Portnum = 0;
		PinCfg.Pinnum = 0;
		PINSEL_ConfigPin(&PinCfg);
		PinCfg.Pinnum = 1;
		PINSEL_ConfigPin(&PinCfg);
		CAN_Init(LPC_CAN1, baudrate);	
		CAN_IRQCmd(LPC_CAN1, CANINT_RIE, ENABLE);
   	}
	else if(2 == CanEnum)
	{
		PinCfg.Funcnum = 2;
		PinCfg.Portnum = 0;
		PinCfg.Pinnum = 4;
		PINSEL_ConfigPin(&PinCfg);
		PinCfg.Pinnum = 5;
		PINSEL_ConfigPin(&PinCfg);	
		CAN_Init(LPC_CAN2, baudrate);
		CAN_IRQCmd(LPC_CAN2, CANINT_RIE, ENABLE);
	}
	CAN_SetAFMode(LPC_CANAF,CAN_AccBP);
	NVIC_EnableIRQ(CAN_IRQn);
}

/***************************************************************************************************************************
**函数名称:	 	FlyCAN_GetCTRLStatus
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
uint32_t FlyCAN_GetCTRLStatus(U32 CanEnum)
{
	LPC_CAN_TypeDef *CANx;
	if(1 == CanEnum)
	{
		CANx = LPC_CAN1;
	}
	else if(2 == CanEnum)
	{
		CANx = LPC_CAN2;
	}
	return(CAN_GetCTRLStatus(CANx,CANCTRL_STS));
}
/***************************************************************************************************************************
**函数名称:	 	FlyCAN_Transmit
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void FlyCAN_Transmit(U32 CanEnum,CanTxMsg *CAN_Msg)
{
	CAN_MSG_Type LPC_CANTxMsg;
	LPC_CAN_TypeDef *CANx;
	CANTxStdMsg2LPCTxMsg(CAN_Msg,&LPC_CANTxMsg);
	if(1 == CanEnum)
	{
		CANx = LPC_CAN1;
	}
	else if(2 == CanEnum)
	{
		CANx = LPC_CAN2;
	}
		
	if(CAN_SendMsg(CANx, &LPC_CANTxMsg) == ERROR)
	{
		printf("\r\n CAN_SendMsg  ERROR");
	}
}
/***************************************************************************************************************************
**函数名称:	 	FlyCAN_DeInit
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void FlyCAN_DeInit(U32 CanEnum)
{
	if(1 == CanEnum)
	{
		CAN_DeInit(LPC_CAN1);
	}
	else if(2 == CanEnum)
	{
		CAN_DeInit(LPC_CAN2);
	}
	NVIC_DisableIRQ(CAN_IRQn);
}
/**************************************************************************************************************************
**函数名称:		SetPwmDutyCycle
**函数功能:		
**入口参数:     无
**返回参数:     无
**************************************************************************************************************************/				
void SetPwmDutyCycle(u16 DutyCycle)
{
	u16 temp;
	printf("\r\n DutyCycle %d",DutyCycle);
	if(DutyCycle > 100)
	{	
		DutyCycle = 100;
	}
	temp = (DutyCycle * LPC_PWM1->MR0) / 100; 
	PWM_MatchUpdate(LPC_PWM1, 3,temp, PWM_MATCH_UPDATE_NEXT_RST); /*PWM 占空比*/
}
/**************************************************************************************************************************
**函数名称:		PendLedPWMInit
**函数功能:		
**入口参数:     无
**返回参数:     无
**************************************************************************************************************************/				
void PendLedPWMInit(void)
{
	PINSEL_CFG_Type PinCfg;
	PinCfg.Funcnum = 1;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Portnum = 2;
	PinCfg.Pinnum = 2;
	PINSEL_ConfigPin(&PinCfg);
	
	LPC_PWM1->PR = (CLKPWR_GetPCLK (CLKPWR_PCLKSEL_PWM1) /1000000 - 1);
	LPC_PWM1->MCR &=~(0x07 << 0);
	LPC_PWM1->MCR |= (0X02 << 0);
	LPC_PWM1->PCR &= ~(0X01 << 3);
	LPC_PWM1->PCR |= (0X01 << 11);
	LPC_PWM1->MR0 = F_100Hz;
	LPC_PWM1->MR3 = 0;	
	LPC_PWM1->LER |= ((0X01 << 0) | (0X01 << 3)); 
	LPC_PWM1->TCR = 0X02;
	LPC_PWM1->TCR = 0X09;
}
/***************************************************************************************************************************
**函数名称:	 	PendLedPWMDeInit
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void PendLedPWMDeInit(void)
{
	PWM_DeInit(LPC_PWM1);
}
/***************************************************************************************************************************
**函数名称:	 	IOFUN_ALLGPIO_CFG
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void IOFUN_ALLGPIO_CFG(void)
{
	
}

/***************************************************************************************************************************
**函数名称:	 	FifoInit
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void FifoInit(void)
{
	
}
/***************************************************************************************************************************
**函数名称:	 	FeedWdt
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void FeedWdt(void)
{
#if(WTD_EN == 1)
	WDT_Feed();
#endif
}
/***************************************************************************************************************************
**函数名称:	 	parameterInit
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void parameterInit(void)
{
	IO_Write(IO_LED0,FALSE);
	IO_Write(IO_LED1,FALSE);
	IO_Write(IO_LED2,FALSE);
	IO_Write(IO_LED3,FALSE);
	IO_Write(IO_LED4,FALSE);
	IO_Write(IO_LED5,FALSE);
	IO_Write(IO_LED6,FALSE);
	IO_Write(IO_LED7,FALSE);
	IO_Write(IO_SDCARDPWR,TRUE);//断电
}
/***************************************************************************************************************************
**函数名称:	 	Time1DeInit
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void Time1DeInit(void)
{
	TIM_Cmd(LPC_TIM1,DISABLE);
	TIM_DeInit(LPC_TIM1);
	NVIC_DisableIRQ(TIMER1_IRQn);	
}
/***************************************************************************************************************************
**函数名称:	 	Time1Init
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void Time1Init(void)
{
   	TIM_TIMERCFG_Type TIM_ConfigStruct;
	TIM_MATCHCFG_Type TIM_MatchConfigStruct;
	TIM_ConfigStruct.PrescaleOption = TIM_PRESCALE_TICKVAL;
	TIM_ConfigStruct.PrescaleValue	= 24;
	TIM_MatchConfigStruct.MatchChannel = TIM_MR1_INT;
	TIM_MatchConfigStruct.IntOnMatch = TRUE;
	TIM_MatchConfigStruct.ResetOnMatch = TRUE;
	TIM_MatchConfigStruct.StopOnMatch = FALSE;
	TIM_MatchConfigStruct.ExtMatchOutputType = TIM_EXTMATCH_NOTHING;
	TIM_MatchConfigStruct.MatchValue = TIME;//
	TIM_Init(LPC_TIM1,TIM_TIMER_MODE,&TIM_ConfigStruct);
	TIM_ConfigMatch(LPC_TIM1,&TIM_MatchConfigStruct);
	NVIC_SetPriority(TIMER1_IRQn,(u8)TIMER1_IRQn);
	NVIC_EnableIRQ(TIMER1_IRQn);
	TIM_Cmd(LPC_TIM1,ENABLE);
}
/***************************************************************************************************************************
**函数名称:	 	TIMER1_IRQHandler
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void TIMER1_IRQHandler(void)
{
   	u8 Status;
   	Status = LPC_TIM1->IR;
	
	LPC_TIM1->IR |= Status;
	
   	if(Status & 0x02)
   	{
		 				
   	}
}
/***************************************************************************************************************************
**函数名称:	 	FlySSP_Init
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void FlySSP_Init(BYTE sspNum,uint32_t clk)
{
	LPC_SSP_TypeDef *SSPx;
	SSP_CFG_Type ConfigStructInit;
	PINSEL_CFG_Type PinCfg;
	if(0X00 == sspNum)
	{
		
		PinCfg.Funcnum = 0X03;
		PinCfg.OpenDrain = 0;
		PinCfg.Pinmode = 0;
		/***********SCK0************/
		PinCfg.Portnum = 1;
		PinCfg.Pinnum = 20;
		PINSEL_ConfigPin(&PinCfg);
		/************MISO***********/
		PinCfg.Portnum = 1;
		PinCfg.Pinnum = 23;
		PINSEL_ConfigPin(&PinCfg);
		/************MOSI***********/
		PinCfg.Portnum = 1;
		PinCfg.Pinnum = 24;
		PINSEL_ConfigPin(&PinCfg);	
		/***********SSEL0***********/
		PinCfg.Funcnum = 0X00;
		PinCfg.Portnum = 1;
		PinCfg.Pinnum = 21;
		PINSEL_ConfigPin(&PinCfg);
		/***************************/
		ConfigStructInit.CPHA = SSP_CPHA_FIRST;//SSP_CPHA_FIRST
		ConfigStructInit.CPOL = SSP_CPOL_LO;//SSP_CPOL_HI
		ConfigStructInit.ClockRate = clk;
		ConfigStructInit.Databit = SSP_DATABIT_8;
		ConfigStructInit.Mode = SSP_MASTER_MODE;
		ConfigStructInit.FrameFormat = SSP_FRAME_SPI;
		SSPx = LPC_SSP0;
	}
	else if(0X01 == sspNum)
	{
		SSPx = LPC_SSP1;
	}
	
	SSP_Init(SSPx,&ConfigStructInit);
	
	SSP_Cmd(SSPx,ENABLE);
}
/***************************************************************************************************************************
**函数名称:	 	FlySSP_DeInit
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void FlySSP_DeInit(BYTE sspNum)
{
	LPC_SSP_TypeDef *SSPx;
	if(0X00 == sspNum)
	{
		SSPx = LPC_SSP0;
	}
	else if(0X01 == sspNum)
	{
		SSPx = LPC_SSP1;
	}
	SSP_DeInit(SSPx);
}
/***************************************************************************************************************************
**函数名称:	 	FlySSP_Init
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void FlySSP_WriteByte(BYTE sspNum,BYTE data)
{
	LPC_SSP_TypeDef *SSPx;
	if(0X00 == sspNum)
	{
		SSPx = LPC_SSP0;
	}
	else if(0X01 == sspNum)
	{
		SSPx = LPC_SSP1;
	}
	while(SET != SSP_GetStatus(SSPx,SSP_STAT_TXFIFO_NOTFULL));
	SSP_SendData(SSPx,data);	
}
/***************************************************************************************************************************
**函数名称:	 	FlySSP_Init
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
BYTE FlySSP_ReadByte(BYTE sspNum)
{
	BYTE Res = 0;
	LPC_SSP_TypeDef *SSPx;
	if(0X00 == sspNum)
	{
		SSPx = LPC_SSP0;
	}
	else if(0X01 == sspNum)
	{
		SSPx = LPC_SSP1;
	}
	while(SET != SSP_GetStatus(SSPx,SSP_STAT_RXFIFO_NOTEMPTY));
	Res = SSP_ReceiveData(SSPx);
	return Res;
}
/***************************************************************************************************************************
**函数名称:	 	FlySSP_ReadWrite
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void FlySSP_ReadWrite(BYTE sspNum,BYTE RW,BYTE *p,UINT len)
{
	LPC_SSP_TypeDef *SSPx;
	SSP_DATA_SETUP_Type dataCfg;
	
	if(0X00 == sspNum)
	{
		SSPx = LPC_SSP0;
	}
	else if(0X01 == sspNum)
	{
		SSPx = LPC_SSP1;
	}
	
	if(0 == RW)
	{
		dataCfg.length = len;
		dataCfg.rx_data = p;
		dataCfg.tx_data = NULL;
		
	}
	else
	{
		dataCfg.length = len;
		dataCfg.tx_data = p;
		dataCfg.rx_data = NULL;
		
	}
	OS_ENTER_CRITICAL();
	SSP_ReadWrite(SSPx,&dataCfg,SSP_TRANSFER_POLLING);
	OS_EXIT_CRITICAL();
}
/***************************************************************************************************************************
**函数名称:	 	FlyEthernetInit
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void FlyEthernetInit(void)
{
	PINSEL_CFG_Type PinCfg;
	EMAC_CFG_Type EMAC_ConfigStruct;
	BYTE MACBuf[] = {0x1E,0x30,0x6c,0xa2,0x45,0x5e};
	PinCfg.Funcnum = 0X01;
	PinCfg.OpenDrain = 0x00;
	PinCfg.Pinmode = 0x00;
	PinCfg.Portnum = 0x01;
	PinCfg.Pinnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 1;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 4;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 8;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 9;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 10;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 14;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 15;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 16;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 17;
	PINSEL_ConfigPin(&PinCfg);
	EMAC_ConfigStruct.Mode = EMAC_MODE_AUTO;
	EMAC_ConfigStruct.pbEMAC_Addr = MACBuf;
	if(SUCCESS == EMAC_Init(&EMAC_ConfigStruct))
	{
		printf("\r\n SUCCESS");
	}
}
/***************************************************************************************************************************
**函数名称:	 	TaskInit
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void TaskInit(void)
{
	ipcStartEvent(EVENT_GLOBAL_MODULE_INIT,0,NULL,0);
}
/***************************************************************************************************************************
**函数名称:	 	chipInit
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void chipInit(void)
{
	SystemInit();
	SysTickInit();
	WatchdogInit();
	DebugInit();	
	PrintWellcomeMsg();	
	parameterInit();
	FlyEthernetInit();
	ipcExchangeInit();
	printf("\r\nparameterInit");
}
/***************************************************************************************************************************
**函数名称:	 	chipDeInit
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void chipDeInit(void)
{
	printf("\r\nchipDeInit");
	SysTickDeInit();
	
	
}


