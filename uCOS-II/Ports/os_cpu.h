/****************************************Copyright (c)****************************************************
**                               Guangzhou ZHIYUAN electronics Co.,LTD.
**                                     
**                                 http://www.embedtools.com
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:               os_cpu.h
** Last modified Date:      2007.01.18
** Last Version:            1.0
** Description:             Header file of the ported code  ��ֲ����ͷ�ļ�
** 
**--------------------------------------------------------------------------------------------------------
** Created By:              Steven Zhou ���ܸ�
** Created date:            2007.01.18
** Version:                 1.0
** Descriptions:            The original version ��ʼ�汾
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             Ni Likao
** Modified date:           2007.10.29
** Version:                 1.1
** Description:             The second version �ڶ���
**
*********************************************************************************************************/

#ifndef  __OS_CPU_H
#define  __OS_CPU_H
                         
#ifdef   OS_CPU_GLOBALS
#define  OS_CPU_EXT
#else
#define  OS_CPU_EXT  extern
#endif


/*********************************************************************************************************
  Date types(Compiler specific)  �������ͣ��ͱ�������أ�                
*********************************************************************************************************/
typedef unsigned char  BOOLEAN;                                         /*  Boolean ��������            */
typedef unsigned char  INT8U;                                           /*  Unsigned  8 bit quantity    */                       
typedef signed   char  INT8S;                                           /*  Signed    8 bit quantity    */                         
typedef unsigned short INT16U;                                          /*  Unsigned 16 bit quantity    */
typedef signed   short INT16S;                                          /*  Signed   16 bit quantity    */
typedef unsigned int   INT32U;                                          /*  Unsigned 32 bit quantity    */
typedef signed   int   INT32S;                                          /*  Signed   32 bit quantity    */
typedef float          FP32;                                            /*  Single precision floating 
                                                                            point                       */
typedef double         FP64;                                            /*  Double precision floating 
                                                                            point                       */

typedef unsigned int   OS_STK;                                          /*  Each stack entry is 32-bit 
                                                                            wide  ��ջ��32λ���        */   
typedef unsigned int   OS_CPU_SR;                                       /*  Define size of CPU status 
                                                                            register (PSR = 32 bits)    */


/*********************************************************************************************************
  Method of critical section management  �ٽ���������                
*********************************************************************************************************/
#define  OS_CRITICAL_METHOD  4


/*********************************************************************************************************
  Other definitions  ��������         
*********************************************************************************************************/
#define  OS_STK_GROWTH        1            
#define  OS_TASK_SW()         OSCtxSw()


/*********************************************************************************************************
  Prototypes(see OS_CPU_A.ASM)  ԭ����������OS_CPU_A.ASM��                
*********************************************************************************************************/
#if OS_CRITICAL_METHOD == 4
	 void OS_ENTER_CRITICAL (void);
     void OS_EXIT_CRITICAL (void);     
#endif

void OSCtxSw (void);
void OSIntCtxSw (void);
void OSStartHighRdy (void);

void OSPendSV (void);

void IntDisAll ( void);
OS_CPU_EXT INT32U OsEnterSum;                   

#endif

/*********************************************************************************************************
  END FILE 
*********************************************************************************************************/
