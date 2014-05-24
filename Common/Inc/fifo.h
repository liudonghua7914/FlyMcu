// -----------------------------------------------------------------------------
// File Name    : fifo.h
// Title        : General FIFO template
// Author       : Seeseawe - Copyright (C) 2008
// Created      : 2008-4-2
// Version      : 0.3
// Target MCU   : Any
// Editor Tabs  : 4
// Editor Line  : 80 chars/line
// -----------------------------------------------------------------------------
// Overview:
/*
	This template provides a convenient way to create a FIFO circular buffer.
*/
// Version History:
/*
    >>>2008-4-6: V0.3 is released.
		C: Changed data type of FIFO position pointer.
    >>>2008-4-3: V0.2 is released.
		+: FIFO_DUMP_DATA(FifoName, Num);
		+: FIFO_PUT_STR(FifoName, ptrData, Num);
		+: FIFO_GET_STR(FifoName, ptrData, Num);
		+: FIFO_PUTS(FifoName, "TxtString");
		+: FIFO_##__NAME##_PutOneInt(Data);
		+: FIFO_##__NAME##_GetOneInt(ptrData);
		-: None
		C: Changed FIFO_PEEK_ONE(ptrData) to FIFO_PEEK_ONE(ptrData, index).
		
    >>>2008-4-2: V0.1 is released.
*/

#ifndef __FIFO_H__
#define __FIFO_H__
//#include "def.h"

/* **********************************************************
*   ģ���˵�������ζ���ģ��                               *
*   �汾��      v1.00beta                                  *
*   ���ߣ�      ��׿Ȼ(OurAVR ɵ����)					   *
*   �������ڣ�  2008��1��19��                              *
* -------------------------------------------------------- *
*  [ʹ��˵��]                                              *
�������뽨��һ��UART���ջ�������������Ҫ������UARTģ���C�ļ��ﴴ��
һ�����ζ��У�����UART_RxFifo,��СΪ128���ֽڣ�
	NEW_FIFO(UART_RxFifo, 128, uint8, uint8);
	
Ȼ����UARTģ���ͷ�ļ��ﵼ����FIFO��API��
	EXPORT_FIFO_API(UART_RxFifo, 128, uint8, uint8);
	
�����������ǿ���ͨ���������ַ��������������
	// ��һ������Data��UART_RxFifo
	1. FIFO_PUT_ONE(UART_RxFifo, Data);
	2. FIFO_UART_RxFifo_PutOne(Data);
	
	// ��ptrData��ȡ��Num�����ݣ�����UART_RxFifo��
	1. FIFO_PUT_STR(UART_RxFifo, ptrData, Num);
	2. FIFO_UART_RxFifo_PutStr(ptrData, Num);
	
	// ��UART_RxFifoȡ��һ�����ݷŵ�ptrData
	1. FIFO_GET_ONE(UART_RxFifo, ptrData);
	2. FIFO_UART_RxFifo_GetOne(ptrData);
	
	// ��UART_RxFifoȡ��Num�����ݷŵ�ptrData
	1. FIFO_GET_STR(UART_RxFifo, ptrData, Num);
	2. FIFO_UART_RxFifo_GetStr(ptrData, Num);
	
	// ͵��һ��UART_RxFifo����index�������ݣ���������浽ptrData
	1. FIFO_PEEK_ONE(UART_RxFifo, ptrData, index);
	2. FIFO_UART_RxFifo_PeekOne(ptrData);
	
	// ����һЩ(Num��)���õ�����
	1. FIFO_DUMP_DATA(UART_RxFifo, Num);
	2. FIFO_UART_RxFifo_DumpData(Num);
	
	// ��ȡUART_RxFifo�ĵ�ǰ������
	1. FIFO_GET_COUNT(UART_RxFifo);
	2. FIFO_UART_RxFifo_GetCount();
	
	// ���UART_RxFifo�Ƿ�Ϊ��
	1. FIFO_IS_EMPTY(UART_RxFifo);
	2. FIFO_UART_RxFifo_IsEmpty();
	
	// ��һ����'\0'��β���ַ�����ӵ�UART_RxFifo
	1. FIFO_PUTS(UART_RxFifo, "Hello World!");
	
��������жϺ����������FIFO��Ӧʹ������ר�ź�����
	FIFO_UART_RxFifo_PutOneInt(Data);
	FIFO_UART_RxFifo_GetOneInt(ptrData);
	
NOTES:
	ͷ�ļ��������ԣ��Ƽ������ļ�(fifo.h)������ģ���ͷ�ļ��У�
	����������ģ����Ҫ���ʸ�fifoʱ��ֻ�������ģ���ͷ�ļ����ɡ�
	�������У�
	A. #include "fifo.h"	// ��uart.h�������fifo.h
	B. #include "uart.h"	// ����Ҫ����UART_RxFifo��ģ���а���uart.h
	
********************************************************** */
		//\
		//Bool FIFO_##__NAME##_PeekOne(__TYPE *pData, __SIZE_t Index)\
		//{\
		//	if (Index < FIFO_##__NAME##_GetCount())\
		//	{\
		//		if ((FIFO_##__NAME##Tail + Index) >= (FIFO_##__NAME##Buffer + __SIZE))\
		//			*pData = *(FIFO_##__NAME##Tail + Index - __SIZE);\
		//		else \
		//			*pData = *(FIFO_##__NAME##Tail + Index);\
		//		return TRUE;\
		//	}\
		//	return FALSE;\
		//}\
		
		/* Dump some data of FIFO */\
		//void FIFO_##__NAME##_DumpData(__SIZE_t Num)\
		//{\
		//	_DI();\
		//	if (Num < FIFO_##__NAME##_GetCount())\
		//	{\
		//		FIFO_##__NAME##Tail += Num;\
		//		if (FIFO_##__NAME##Tail >= FIFO_##__NAME##Buffer + __SIZE) {\
		//			FIFO_##__NAME##Tail -= __SIZE;\
		//		}\
		//	}\
		//	else \
		//	{\
		//		FIFO_##__NAME##Head = FIFO_##__NAME##Buffer;\
		//		FIFO_##__NAME##Tail = FIFO_##__NAME##Buffer;\
		//	}\
		//	_EI();\
		//}\
		

/* -----------------------------------------------------------------------------
   NAME: NEW_FIFO
   FUNCTION: Create a new FIFO
   INPUTS:
		__NAME: FIFO Name;
		__SIZE: FIFO Size;
		__TYPE: The data type of FIFO data member;
		__SIZE_t: The data type of FIFO size.
   OUTPUT:
		Methods for FIFO_PUT_ONE, FIFO_GET_ONE, FIFO_PEEK_ONE, FIFO_DUMP_DATA,
		FIFO_PUT_STR, FIFO_GET_STR, FIFO_PUTS, FIFO_GET_COUNT and FIFO_IS_EMPTY.
----------------------------------------------------------------------------- */
//#define NULL  0


#define NEW_FIFO(__NAME,__SIZE,__TYPE,__SIZE_t)	\
		__TYPE   FIFO_##__NAME##Buffer[(__SIZE)];\
		__TYPE  *FIFO_##__NAME##Tail = FIFO_##__NAME##Buffer;\
		__TYPE  *FIFO_##__NAME##Head = FIFO_##__NAME##Buffer;\
		\
		/* Get the current data count of FIFO outside ISR. */\
		__SIZE_t FIFO_##__NAME##_GetCount(void)\
		{\
			__SIZE_t temp;\
			_DI();\
			if (FIFO_##__NAME##Head >= FIFO_##__NAME##Tail)\
				temp = FIFO_##__NAME##Head - FIFO_##__NAME##Tail;\
			else \
				temp = FIFO_##__NAME##Head + __SIZE - FIFO_##__NAME##Tail;\
			_EI();\
			return temp;\
		}\
		\
		/* Get the current Idel room count of FIFO outside ISR. */\
		__SIZE_t FIFO_##__NAME##_GetIdelCount(void)\
		{\
			__SIZE_t temp;\
			_DI();\
			if (FIFO_##__NAME##Head >= FIFO_##__NAME##Tail)\
				temp = ((__SIZE - 1) -(FIFO_##__NAME##Head - FIFO_##__NAME##Tail));\
			else \
				temp = (FIFO_##__NAME##Tail - 1 - FIFO_##__NAME##Head);\
			_EI();\
			return temp;\
		}\
		\
		/* Put one data in FIFO outside ISR. */\
		BOOL FIFO_##__NAME##_PutOne(__TYPE Data)\
		{\
			__SIZE_t DataCount;\
			_DI();\
			if (FIFO_##__NAME##Head >= FIFO_##__NAME##Tail)\
				DataCount = FIFO_##__NAME##Head - FIFO_##__NAME##Tail;\
			else \
				DataCount =__SIZE  +  FIFO_##__NAME##Head - FIFO_##__NAME##Tail;\
			_EI();\
			if (DataCount < (__SIZE - 1))\
			{\
				_DI();\
				*FIFO_##__NAME##Head = Data;\
				/* "*FIFO_##__NAME##Head = Data;"��Ҫ�ڹ��ж��½���,�ַ��͵Ĳ�������������� */\
				if (FIFO_##__NAME##Head == FIFO_##__NAME##Buffer + (__SIZE - 1))\
					FIFO_##__NAME##Head = FIFO_##__NAME##Buffer;\
				else \
					FIFO_##__NAME##Head++;\
				_EI();\
				return TRUE;\
			}\
			return FALSE;\
		}\
		\
		/* Get one data from FIFO outside ISR. */\
		BOOL FIFO_##__NAME##_GetOne(__TYPE *pData)\
		{\
			_DI();\
			if (FIFO_##__NAME##Tail != FIFO_##__NAME##Head)\
			{\
				*pData = *FIFO_##__NAME##Tail;\
				if (FIFO_##__NAME##Tail == FIFO_##__NAME##Buffer + (__SIZE - 1))\
					FIFO_##__NAME##Tail = FIFO_##__NAME##Buffer;\
				else \
					FIFO_##__NAME##Tail++;\
				_EI();\
				return TRUE;\
			}\
			_EI();\
			return FALSE;\
		}\
		/* take a look the data with Index*/\
		BOOL FIFO_##__NAME##_PeekOne(__TYPE *pData, __SIZE_t Index)\
		{\
			if (Index < FIFO_##__NAME##_GetCount())\
			{\
				if ((FIFO_##__NAME##Tail + Index) >= (FIFO_##__NAME##Buffer + __SIZE))\
					*pData = *(FIFO_##__NAME##Tail + Index - __SIZE);\
				else \
					*pData = *(FIFO_##__NAME##Tail + Index);\
				return TRUE;\
			}\
			return FALSE;\
		}\
		/* Dump some data of FIFO */\
		void FIFO_##__NAME##_DumpData(__SIZE_t Num)\
		{\
			_DI();\
			if (Num < FIFO_##__NAME##_GetCount())\
			{\
				FIFO_##__NAME##Tail += Num;\
				if (FIFO_##__NAME##Tail >= FIFO_##__NAME##Buffer + __SIZE) {\
					FIFO_##__NAME##Tail -= __SIZE;\
				}\
			}\
			else \
			{\
				FIFO_##__NAME##Head = FIFO_##__NAME##Buffer;\
				FIFO_##__NAME##Tail = FIFO_##__NAME##Buffer;\
			}\
			_EI();\
		}\
		/* Put a string of data in FIFO */\
		BOOL FIFO_##__NAME##_PutStr(__TYPE *pData, __SIZE_t Num)\
		{\
			if ((Num > FIFO_##__NAME##_GetIdelCount()) || (pData == NULL))\
				return FALSE;\
			while(Num--)\
			{\
				_DI();\
				*FIFO_##__NAME##Head = *pData;\
				if (FIFO_##__NAME##Head == FIFO_##__NAME##Buffer + (__SIZE - 1))\
					FIFO_##__NAME##Head = FIFO_##__NAME##Buffer;\
				else \
					FIFO_##__NAME##Head++;\
				pData++;\
				_EI();\
			}\
			return TRUE;\
		}\
		BOOL FIFO_##__NAME##_GetStr(__TYPE *pData, __SIZE_t Num)\
		{\
			if ((Num > FIFO_##__NAME##_GetCount()) || (pData == NULL))\
				return FALSE;\
			while(Num--)\
			{\
				*pData = *FIFO_##__NAME##Tail;\
				_DI();\
				if (FIFO_##__NAME##Tail == FIFO_##__NAME##Buffer + (__SIZE - 1))\
					FIFO_##__NAME##Tail = FIFO_##__NAME##Buffer;\
				else \
					FIFO_##__NAME##Tail++;\
				pData++;\
				_EI();\
			}\
			return TRUE;\
		}\
		BOOL FIFO_##__NAME##_IsEmpty(void)\
		{\
			if (FIFO_##__NAME##Tail == FIFO_##__NAME##Head) return TRUE;\
			else return FALSE;\
		} \
		extern __inline void FIFO_##__NAME##_PutOneInt(__TYPE Data)\
		{\
			if ((FIFO_##__NAME##Head+1) !=  FIFO_##__NAME##Tail) {\
				if (((FIFO_##__NAME##Head+1)- __SIZE) !=  FIFO_##__NAME##Tail) {\
					*FIFO_##__NAME##Head = Data;\
					if (FIFO_##__NAME##Head == FIFO_##__NAME##Buffer + (__SIZE - 1))\
						FIFO_##__NAME##Head = FIFO_##__NAME##Buffer;\
					else \
						FIFO_##__NAME##Head++;\
				}\
			}\
		}\
		extern __inline BOOL FIFO_##__NAME##_GetOneInt(__TYPE *pData)\
		{\
			if (FIFO_##__NAME##Tail == FIFO_##__NAME##Head) return FALSE;\
			*pData = *FIFO_##__NAME##Tail; \
			if (FIFO_##__NAME##Tail == FIFO_##__NAME##Buffer + (__SIZE - 1)) {\
				FIFO_##__NAME##Tail = FIFO_##__NAME##Buffer;\
			}\
			else {\
				FIFO_##__NAME##Tail++;\
			}\
			return TRUE;\
		}\
		
		

#define EXPORT_FIFO_API(__NAME,__SIZE,__TYPE,__SIZE_t)  \
		extern __TYPE   FIFO_##__NAME##Buffer[(__SIZE)];\
		extern __TYPE  *FIFO_##__NAME##Tail;\
		extern __TYPE  *FIFO_##__NAME##Head;\
		extern BOOL FIFO_##__NAME##_PutOne(__TYPE Data);\
		extern BOOL FIFO_##__NAME##_GetOne(__TYPE *pData);\
		extern BOOL FIFO_##__NAME##_PeekOne(__TYPE *pData, __SIZE_t Index);\
		extern void FIFO_##__NAME##_DumpData(__SIZE_t Num);\
		extern BOOL FIFO_##__NAME##_PutStr(__TYPE *pData, __SIZE_t Num);\
		extern BOOL FIFO_##__NAME##_GetStr(__TYPE *pData, __SIZE_t Num);\
		extern BOOL FIFO_##__NAME##_IsEmpty(void);\
		extern __SIZE_t FIFO_##__NAME##_GetCount(void);\
		extern __SIZE_t FIFO_##__NAME##_GetIdelCount(void);\
		extern __inline void FIFO_##__NAME##_PutOneInt(__TYPE Data)\
		{\
			if ((FIFO_##__NAME##Head+1) !=  FIFO_##__NAME##Tail) {\
				if (((FIFO_##__NAME##Head+1)- __SIZE) !=  FIFO_##__NAME##Tail) {\
					*FIFO_##__NAME##Head = Data;\
					if (FIFO_##__NAME##Head == FIFO_##__NAME##Buffer + (__SIZE - 1))\
						FIFO_##__NAME##Head = FIFO_##__NAME##Buffer;\
					else \
						FIFO_##__NAME##Head++;\
				}\
			}\
		}\
		extern __inline BOOL FIFO_##__NAME##_GetOneInt(__TYPE *pData)\
		{\
			if (FIFO_##__NAME##Tail == FIFO_##__NAME##Head) return FALSE;\
			*pData = *FIFO_##__NAME##Tail; \
			if (FIFO_##__NAME##Tail == FIFO_##__NAME##Buffer + (__SIZE - 1)) {\
				FIFO_##__NAME##Tail = FIFO_##__NAME##Buffer;\
			}\
			else {\
				FIFO_##__NAME##Tail++;\
			}\
			return TRUE;\
		}\
		

#define FIFO_INIT(__NAME)    _DI();FIFO_##__NAME##Tail = FIFO_##__NAME##Buffer;\
							 FIFO_##__NAME##Head = FIFO_##__NAME##Buffer;_EI();

#define FIFO_PUT_ONE_INT(__NAME,__DATA)      FIFO_##__NAME##_PutOneInt(__DATA)
#define FIFO_GET_ONE_INT(__NAME,__ADDR)      FIFO_##__NAME##_GetOneInt(__ADDR)
#define FIFO_PUT_ONE(__NAME,__DATA)          FIFO_##__NAME##_PutOne(__DATA)
#define FIFO_GET_ONE(__NAME,__ADDR)          FIFO_##__NAME##_GetOne(__ADDR)
#define FIFO_PEEK_ONE(__NAME,__ADDR,__IDX)   FIFO_##__NAME##_PeekOne(__ADDR,__IDX)
#define FIFO_DUMP_DATA(__NAME,__NUM)		 FIFO_##__NAME##_DumpData(__NUM)
#define FIFO_GET_COUNT(__NAME)               FIFO_##__NAME##_GetCount()
#define FIFO_GET_IDEL_COUNT(__NAME)          FIFO_##__NAME##_GetIdelCount()
#define FIFO_IS_EMPTY(__NAME)              	 FIFO_##__NAME##_IsEmpty()
#define FIFO_PUT_STR(__NAME,__ADDR,__NUM)	 FIFO_##__NAME##_PutStr(__ADDR,__NUM)
#define FIFO_GET_STR(__NAME,__ADDR,__NUM)	 FIFO_##__NAME##_GetStr(__ADDR,__NUM)
#define FIFO_PUTS(__NAME,__ADDR)			 FIFO_##__NAME##_PutStr(__ADDR,sizeof(__ADDR))

#endif
// -------------------------------- END OF FILE --------------------------------
