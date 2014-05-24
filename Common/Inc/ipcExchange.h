#ifndef _IPC_EXCHANGE_H_
#define _IPC_EXCHANGE_H_
#include "def.h"




#ifndef BV
	#define BV(bitx)			((u32)1<<(bitx))
#endif
#ifndef cbi
	#define cbi(REG, bitx)	REG &= ~(BV(bitx))
#endif
#ifndef sbi
	#define sbi(REG, bitx)	REG |= (BV(bitx))
#endif

// Check if the bit is '1'
#define bit_is_set(REG, bitx)   (((REG) & BV(bitx)) != 0)
// Check if the bit is '0'
#define bit_is_clr(REG, bitx)   (((REG) & BV(bitx)) == 0)

// MIN/MAX/ABS macros
#define SMALLER(a, b)			(((a) < (b))?(a):(b))
#define LARGER(a, b)			(((a) > (b))?(a):(b))
#define ABS(x)					((x > 0)?(x):(-x))




	typedef struct _IPC_EXCHANGE_INFO{
		u32 EventSet[16];
	}IPC_EXCHANGE_INFO;

	#ifndef		_IPC_EXCHANGE_GLOBAL_
		#define		IPC_EXCHANGE_GLOBAL		extern

	#else
		#define		IPC_EXCHANGE_GLOBAL	
		//¾Ö²¿ÉêÃ÷:.........	
	#endif

	
IPC_EXCHANGE_GLOBAL IPC_EXCHANGE_INFO ipcExchangeInfo;

IPC_EXCHANGE_GLOBAL void ipcExchangeEvent(ULONG enumWhatEvent,ULONG lPara,BYTE *p,ULONG length);

IPC_EXCHANGE_GLOBAL	void ipcStartEvent(ULONG enumWhatEvent,ULONG lPara,BYTE *p,ULONG length);
IPC_EXCHANGE_GLOBAL	void ipcClearEvent(UINT32 sourceEvent);
IPC_EXCHANGE_GLOBAL	BOOL ipcWhatEventOn(UINT32 sourceEvent);

IPC_EXCHANGE_GLOBAL void ipcExchangeInit(void);

#endif

