#ifndef 	__FIFO_H__
#define		__FIFO_H__

#include "def.h"

typedef struct
{
	UINT16 rear;
	UINT16 front;
	UINT16 size;
	DataType *pdata;
	UINT16 count;
}T_Queue,*P_Queue;
























#endif

