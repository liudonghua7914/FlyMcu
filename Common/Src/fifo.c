#include <string.h>
#include <def.h>

#include "fifo.h"

/***************************************************************************************************************************
**��������:	 	QueueInit	
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
static void QueueInit(P_Queue pQueue,DataType *p,UINT16 size)
{
	pQueue->front = pQueue->rear = 0;
	pQueue->count = 0;
	pQueue->pdata = p;
	pQueue->size = size;
	memset((void *)pQueue->pdata,0,pQueue->size);
}
/***************************************************************************************************************************
**��������:	 	QueueEmpty	
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
static UINT16 QueueEmpty(P_Queue pQueue)
{
	return (pQueue->count);
}
/***************************************************************************************************************************
**��������:	 	QueueEmpty	
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
static BOOL QueueFull(P_Queue pQueue)
{
	return (BOOL)(pQueue->count == pQueue->size);
}
/***************************************************************************************************************************
**��������:	 	EnQueue	
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
static void EnQueue(P_Queue pQueue,DataType Dat)
{
	OS_ENTER_CRITICAL();
	if(QueueFull(pQueue))
	{
		printf("\r\n overflow");
	}
	pQueue->count++;
	pQueue->pdata[pQueue->rear] = Dat;
	pQueue->rear = (pQueue->rear + 1) % pQueue->size;
	OS_EXIT_CRITICAL();
}
/***************************************************************************************************************************
**��������:	 	DeQueue	
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
static DataType DeQueue(P_Queue pQueue)
{
	DataType tmp;
	OS_ENTER_CRITICAL();
	tmp = pQueue->pdata[pQueue->front];
	pQueue->count--;
	pQueue->front = (pQueue->front + 1) % pQueue->size;
	OS_EXIT_CRITICAL();
	return tmp;
}
/***************************************************************************************************************************
**��������:	 	QueueFront	
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
static DataType QueueFront(P_Queue pQueue)
{
	return pQueue->pdata[pQueue->front];
}




