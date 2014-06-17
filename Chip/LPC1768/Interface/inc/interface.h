#ifndef _INTERFACE_H_
#define _INTERFACE_H_
#include "config.h"
#include "lpc_types.h"
#include "lpc17xx_clkpwr.h"
#include "debug_frmwrk.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_exti.h"
#include "lpc17xx_i2c.h"
#include "lpc17xx_pwm.h"
#include "lpc17xx_systick.h"
#include "lpc17xx_can.h"
#include "lpc17xx_ssp.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_wdt.h"
#include "lpc17xx_rtc.h"
#include "IAP.h"







#define	 FLY_SSP0		0x00
#define	 FLY_SSP1		0x01


#define	 FLYREAD		0X00
#define	 FLYWRITE		0X01

#define	SSP_100KHz		(100000)
#define	SSP_400KHz		(400000)
#define	SSP_1MHz		(1000000)

#define	 DEBUG_PORT_SELECT		0



#if(0 == DEBUG_PORT_SELECT)	
	#define		FUNCNUM		0X01
	#define		PORTNUM		0
	#define		PINNUM1		2	
	#define		PINNUM2		3	
	#define		DEBUG_PORT	(LPC_UART0)
#elif(1 == DEBUG_PORT_SELECT)
	#define		DEBUG_PORT	(LPC_UART1)
#elif(2 == DEBUG_PORT_SELECT)
	#define		FUNCNUM		0X01
	#define		PORTNUM		0
	#define		PINNUM1		10
	#define		PINNUM2		11	
	#define		DEBUG_PORT	(LPC_UART2)
#elif(3 == DEBUG_PORT_SELECT)
	#define		DEBUG_PORT	(LPC_UART3)
#endif


#define	TIME_1US		(1)
#define	TIME_10US		(10 * TIME_1US)
#define	TIME_100US		(10 * TIME_10US)
#define	TIME_1MS		(10 * TIME_100US)
#define	TIME_10MS		(10 * TIME_1MS)
#define	TIME_100MS		(10 * TIME_10MS)

#define	TIME		(TIME_1MS)

#define	PWM_5Hz	    (200000)
#define	PWM_10Hz	(100000)	
#define	PWM_100Hz	(10000)	
#define	PWM_1KHz	(1000)	
#define	PWM_2KHz	(500)		
#define	PWM_5KHz	(200)		
#define	PWM_10KHz	(100)

#define	PWM_TIME	(PWM_5Hz / TIME)
#define	PWM_TIME_H	( (60 * PWM_5Hz) / 100 / TIME )




#define CAN_ID_STD                 ((uint32_t)0x00000000)  /*!< Standard Id */
#define CAN_ID_EXT                 ((uint32_t)0x00000004)  /*!< Extended Id */
#define CAN_RTR_DATA               ((uint32_t)0x00000000)  /*!< Data frame */
#define CAN_RTR_REMOTE             ((uint32_t)0x00000002)  /*!< Remote frame */
////////////////////////////////////////////////////////////////////////////////////////
#define 	T_1MS		(1)
#define 	T_10MS		(10)
#define 	T_100MS		(100)
#define 	T_1S		(1000)
#define 	T_1MM		(T_1S*60)

#define		F_100Hz		10000	
#define		F_1KHz		1000
#define		F_10KHz		100

////////////////////////////////////////////////////////////////////////////////////////
//Watchodog time out in 5 seconds,最大0xffff ffff微秒约	4295s,约71分钟

#define	WTD_EN						0
#define	WDT_1SS						1000000
#define WDT_1MM						(WDT_1SS*60)
#define WDT_1HH						(WDT_1MM*60)

#define	WDT_PRE						(60)

#define WDT_TIMEOUT 				(WDT_1MM)
#define WDT_TIMEOUT_INSLEEPMODE 	(WDT_1HH)
////////////////////////////////////////////////////////////////////////////////////////
#define PIN_INPUT				(0)
#define PIN_OUTPUT				(1)

#define ACC_IN_PORT				(PINSEL_PORT_0)
#define ACC_IN_PIN				(PINSEL_PIN_9)

////////////////////////////////////////////////////////////////////////////////////////


#define IO_LED0_PORT			(PINSEL_PORT_2)
#define IO_LED0_PIN				(PINSEL_PIN_0)	

#define IO_LED1_PORT			(PINSEL_PORT_2)
#define IO_LED1_PIN				(PINSEL_PIN_1)	

#define IO_LED2_PORT			(PINSEL_PORT_2)
#define IO_LED2_PIN				(PINSEL_PIN_2)	

#define IO_LED3_PORT			(PINSEL_PORT_2)
#define IO_LED3_PIN				(PINSEL_PIN_3)	

#define IO_LED4_PORT			(PINSEL_PORT_2)
#define IO_LED4_PIN				(PINSEL_PIN_4)	

#define IO_LED5_PORT			(PINSEL_PORT_2)
#define IO_LED5_PIN				(PINSEL_PIN_5)	

#define IO_LED6_PORT			(PINSEL_PORT_2)
#define IO_LED6_PIN				(PINSEL_PIN_6)	

#define IO_LED7_PORT			(PINSEL_PORT_2)
#define IO_LED7_PIN				(PINSEL_PIN_7)	



#define	IO_SDCS_PORT			(PINSEL_PORT_1)		
#define	IO_SDCS_PIN				(PINSEL_PIN_21)	

#define	IO_SDCD_PORT			(PINSEL_PORT_3)		
#define	IO_SDCD_PIN				(PINSEL_PIN_25)	

#define	IO_SDPWR_PORT			(PINSEL_PORT_3)		
#define	IO_SDPWR_PIN			(PINSEL_PIN_26)	



#define	IIC_SCL_PORT			(PINSEL_PORT_0)
#define	IIC_SCL_PIN				(PINSEL_PIN_28)

#define	IIC_SDA_PORT			(PINSEL_PORT_0)
#define	IIC_SDA_PIN				(PINSEL_PIN_27)

typedef enum
{
	IO_LED0 = 0,
	IO_LED1,
	IO_LED2,
	IO_LED3,
	IO_LED4,
	IO_LED5,
	IO_LED6,
	IO_LED7,
	IO_SDCARDCS,
	IO_SDCARDCD,
	IO_SDCARDPWR,
	IO_I2C_SCL,
	IO_I2C_SDA,
	IO_MAX,
}enumIODef;


typedef struct
{
  uint32_t StdId;
  uint32_t ExtId;
  uint8_t IDE;   
  uint8_t RTR; 
  uint8_t DLC; 
  uint8_t Data[8];
  uint8_t FMI; 
}CanRxMsg;

typedef struct
{
  uint32_t StdId;
  uint32_t ExtId;
  uint8_t IDE; 
  uint8_t RTR; 
  uint8_t DLC; 
  uint8_t Data[8];
} CanTxMsg;


typedef struct _INTERFACE_INFO
{
	U32 SysTickCount;
	U32 IIcCount;
	U32 CanCount;
	U32 CanCount2;
	U8  OpenUartDevice;
	U32 UartCount;
}INTERFACE_INFO;

	#ifndef		_INTERFACE_GLOBAL_
		#define		INTERFACE_GLOBAL		extern
	#else
		#define		INTERFACE_GLOBAL	
		//局部申明:.........	

	
	#endif

INTERFACE_GLOBAL INTERFACE_INFO interfaceInfo;
		


INTERFACE_GLOBAL void _DI(void);
INTERFACE_GLOBAL void _EI(void);



INTERFACE_GLOBAL BOOL IO_Read(U32 IOEnum);
INTERFACE_GLOBAL void IO_Write(U32 IOEnum,BOOL status);
INTERFACE_GLOBAL void IO_WriteExtended(uint8_t Port,uint8_t Pin,uint8_t status);
INTERFACE_GLOBAL void FlyCAN_DeInit(U32 CanEnum);
INTERFACE_GLOBAL void FlyCAN_Init(U32 CanEnum,U32 baudrate,U32 *pIDFilterList);

INTERFACE_GLOBAL void FlyUART_DeInit(U32 ComEnum);
INTERFACE_GLOBAL void FlyUART_Init(U32 ComEnum,U32 baudrate);


INTERFACE_GLOBAL void chipInit(void);
INTERFACE_GLOBAL void chipDeInit(void);
INTERFACE_GLOBAL uint32_t ReadUserTimer(uint32_t *Timer);
INTERFACE_GLOBAL void ResetUserTimer(uint32_t *Timer);
INTERFACE_GLOBAL void SetUserTimer(uint32_t *timer,uint32_t T);
INTERFACE_GLOBAL uint32_t GetTickCount(void);
INTERFACE_GLOBAL void Delayms(uint32_t Time);
INTERFACE_GLOBAL uint16_t GetAdcChannelValue(uint8_t Channel);
INTERFACE_GLOBAL void SetPwmDutyCycle(u16 DutyCycle);
INTERFACE_GLOBAL void UserResetSystem(void);

INTERFACE_GLOBAL void GoT0WhatStatus(uint32_t Address);
INTERFACE_GLOBAL void parameterInit(void);
INTERFACE_GLOBAL void RTC4Wakeup2FeedDog(u32 alarmTime);
INTERFACE_GLOBAL void FeedWdt(void);
INTERFACE_GLOBAL void IntoDeepSleep(void);
INTERFACE_GLOBAL void IOFUN_ALLGPIO_CFG(void);
INTERFACE_GLOBAL void FlyCAN_Transmit(U32 CanEnum,CanTxMsg *CAN_Msg);
INTERFACE_GLOBAL uint32_t FlyCAN_GetCTRLStatus(U32 CanEnum);
INTERFACE_GLOBAL void AdcsInit(void);
INTERFACE_GLOBAL void DebugInit(void);
INTERFACE_GLOBAL void PrintWellcomeMsg(void);
INTERFACE_GLOBAL void DebugInit(void);
INTERFACE_GLOBAL void FlyUART_Transmit(U32 ComEnum,uint8_t pdata);
INTERFACE_GLOBAL void FlyUART_TransmitToCar(uint8_t Num,uint8_t *pdata,uint8_t len);
INTERFACE_GLOBAL void FlySetFlashSaveParameter(void);
INTERFACE_GLOBAL void FlyGetFlashSaveParameter(void);
INTERFACE_GLOBAL void WdtForWakeUp(uint32_t Time);
INTERFACE_GLOBAL void UartForCarInit(uint8_t Num,UART_CFG_Type UARTConfigStruct,BOOL IrqEnable);
INTERFACE_GLOBAL void UartForCarDeInit(uint8_t Num,BOOL IrqEnable);
INTERFACE_GLOBAL void TaskInit(void);
INTERFACE_GLOBAL void FlySSP_Init(BYTE sspNum,uint32_t clk);
INTERFACE_GLOBAL BYTE FlySSP_ReadByte(BYTE sspNum);
INTERFACE_GLOBAL void FlySSP_WriteByte(BYTE sspNum,BYTE data);
INTERFACE_GLOBAL void FlySSP_DeInit(BYTE sspNum);
INTERFACE_GLOBAL void FlySSP_ReadWrite(BYTE sspNum,BYTE RW,BYTE *p,UINT len);

INTERFACE_GLOBAL void I2CStart(void);
INTERFACE_GLOBAL void I2CStop(void);
INTERFACE_GLOBAL BOOL WaitI2CACK(void);
INTERFACE_GLOBAL BYTE I2CReadByte(void);
INTERFACE_GLOBAL void I2CWriteByte(BYTE data);
#endif

