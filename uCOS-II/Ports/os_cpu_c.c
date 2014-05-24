/****************************************Copyright (c)****************************************************
**                               Guangzhou ZHIYUAN electronics Co.,LTD.
**                                     
**                                 http://www.embedtools.com
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:               os_cpu_c.c
** Last modified Date:      2007.01.18
** Last Version:            1.0
** Description:             The C functions that a uC/OS-II port requires    uC/OS-II??????C??		 
** 
** Created by:				LDH
** Created Date:		    2013.02.27
*********************************************************************************************************/
#define  OS_CPU_GLOBALS
#include <includes.h>


/********************************************************************************************************
  Local variables ????
********************************************************************************************************/
#if OS_TMR_EN > 0
    static INT16U OSTmrCtr;
#endif

/*********************************************************************************************************
** Function name:			OSInitHookBegin
** Descriptions:			The hook function called by OSInit() at the beginning of OSInit()
**                          OSInit()???????????????    
** Input parameters:		None ?
** Output parameters:		None ?
** Returned value:		    None ?
** Created by:				LDH
** Created Date:		    2013.02.27
**-------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#if OS_CPU_HOOKS_EN > 0 && OS_VERSION > 203
    void OSInitHookBegin (void)
    {
        #if OS_TMR_EN > 0
            OSTmrCtr = 0;
        #endif
    }
#endif


/*********************************************************************************************************
** Function name:			OSInitHookEnd
** Descriptions:			The hook function called by OSInit() at the end of OSInit()
**                          OSInit()????????????    
** Input parameters:		None ?
** Output parameters:		None ?
** Returned value:		    None ?
** Created by:				LDH
** Created Date:		    2013.02.27
**-------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#if OS_CPU_HOOKS_EN > 0 && OS_VERSION > 203
    void OSInitHookEnd (void)
    {
    }
#endif

/*********************************************************************************************************
** Function name:			OSTaskCreateHook
** Descriptions:			The hook function called when a task is created
**                          ?????????????
** Input parameters:		ptcb
** Output parameters:		None ?
** Returned value:		    None ?
** Created by:				LDH
** Created Date:		    2013.02.27
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#if OS_CPU_HOOKS_EN > 0
    void OSTaskCreateHook (OS_TCB  *ptcb)
    {
	    (void)ptcb;                                                     /*  Prevent compiler warning    */
                                                                        /*  ??????                */
    }
#endif

/*********************************************************************************************************
** Function name:			OSTaskDelHook
** Descriptions:			The hook function called when a task is deleted
**                          ????????????
** Input parameters:		ptcb
** Output parameters:		None ?
** Returned value:		    None ?
** Created by:				LDH
** Created Date:		    2013.02.27
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#if OS_CPU_HOOKS_EN > 0
    void  OSTaskDelHook (OS_TCB  *ptcb)
    {
        (void)ptcb;                                                     /*  Prevent compiler warning    */
                                                                        /*  ??????                */
    }
#endif

/*********************************************************************************************************
** Function name:			OSTaskIdleHook
** Descriptions:			The hook function called by the idle task
**                          ???????????
** Input parameters:		None ?
** Output parameters:		None ?
** Returned value:		    None ?
** Created by:				LDH
** Created Date:		    2013.02.27
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#if OS_CPU_HOOKS_EN > 0 && OS_VERSION >= 251
    void OSTaskIdleHook (void)
    {
    }
#endif

/*********************************************************************************************************
** Function name:			OSTaskStatHook
** Descriptions:			The hook function called every second by the statistics task 
**                          ??????????????
** Input parameters:		None ?
** Output parameters:		None ?
** Returned value:		    None ?
** Created by:				LDH
** Created Date:		    2013.02.27
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#if OS_CPU_HOOKS_EN > 0
    void OSTaskStatHook (void)
    {
    }
#endif

/*********************************************************************************************************
** Function name:			OSTaskStkInit
** Descriptions:			This function is called by either OSTaskCreate() or OSTaskCreateExt() to 
**                          initialize the stack frame of the task being created.  
**                          This function is highly processor specific.
**                          OSTaskCreate() ? OSTaskCreateExt()??????????????????
**                          ???????????? 
** Input parameters:		task:  A pointer to the task code   ???????
**                          p_arg: A pointer to a user supplied data area that will be passed to the task
**                                  when the task first executes.
**                                  ????????????,???????????? 
**                          ptos:   A pointer to the top of the stack. For this port, it's the highest  
**                                  valid address of the stack. 
**                                  ???????????,?????????????
**                          opt:    Specified options that can be used to alter the behavior of 
*                                   OSTaskStkInit().(see uCOS_II.H for OS_TASK_OPT_xxx).
**                                  ????,?????OSTaskStkInit()??????
**                                  (?? uCOS_II.H ?? OS_TASK_OPT_xxx).
** Output parameters:		None ?
** Returned value:	        The new location of top-of-stack  ???????         
** Created by:				LDH
** Created Date:		    2013.02.27
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
OS_STK *OSTaskStkInit (void  (*task)(void  *parg), void  *parg, OS_STK  *ptos, INT16U  opt)
{
    OS_STK *stk;


    (void)opt;                                                          /*  'opt' is not used, prevent  */
                                                                        /*  warning ???'opt',       */
                                                                        /*  ??????                */
                                                                            
    stk = ptos;                                                         /*  Load stack pointer          */
                                                                        /*  ??????                */                                    

                                                                        /*  Registers stacked as if     */
                                                                        /*  auto-saved on exception     */             
	                                                                    /*  ?????,????????*/

    *(stk) = (INT32U)0x01000000L;                                       /*  xPSR                        */ 
    *(--stk) = (INT32U)task;                                            /*  Entry Point of the task     */
                                                                        /*  ??????                */
    *(--stk) = (INT32U)0xFFFFFFFEL;                                     /*  R14 (LR)  (init value will  */
                                                                        /*  cause fault if ever used)   */
                                                                           
    *(--stk) = (INT32U)0x12121212L;                                     /*  R12                         */
    *(--stk) = (INT32U)0x03030303L;                                     /*  R3                          */
    *(--stk) = (INT32U)0x02020202L;                                     /*  R2                          */
    *(--stk) = (INT32U)0x01010101L;                                     /*  R1                          */
    *(--stk) = (INT32U)parg;                                            /*  R0 : argument  ????     */
                                                                        /*  Remaining registers saved on*/
                                                                        /*  process stack               */   
	                                                                    /*  ???????????      */

    *(--stk) = (INT32U)0x11111111L;                                     /*  R11                         */
    *(--stk) = (INT32U)0x10101010L;                                     /*  R10                         */
    *(--stk) = (INT32U)0x09090909L;                                     /*  R9                          */
    *(--stk) = (INT32U)0x08080808L;                                     /*  R8                          */
    *(--stk) = (INT32U)0x07070707L;                                     /*  R7                          */
    *(--stk) = (INT32U)0x06060606L;                                     /*  R6                          */
    *(--stk) = (INT32U)0x05050505L;                                     /*  R5                          */
    *(--stk) = (INT32U)0x04040404L;                                     /*  R4                          */

    return(stk);
}

/*********************************************************************************************************
** Function name:			OSTaskSwHook
** Descriptions:			The hook function called when a task switch is performed.
**                          ?????????????    
** Input parameters:		None ?
** Output parameters:		None ?
** Returned value:		    None ?
** Created by:				LDH
** Created Date:		    2013.02.27
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
void OSTaskSwHook (void)
{

}

/*********************************************************************************************************
** Function name:			OSTCBInitHook
** Descriptions:			The hook function called by OS_TCBInit()
**                          OS_TCBInit()???????    
** Input parameters:		ptcb
** Output parameters:		None ?
** Returned value:		    None ?
** Created by:				LDH
** Created Date:		    2013.02.27
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#if (OS_CPU_HOOKS_EN > 0) && (OS_VERSION > 203)
    void OSTCBInitHook (OS_TCB  *ptcb)
    {
        (void)ptcb;                                                     /*  Prevent compiler warning    */
                                                                        /*  ??????                */
    }
#endif

/*********************************************************************************************************
** Function name:			OSTimeTickHook
** Descriptions:			The hook function called every tick
**                          ???????????    
** Input parameters:		ptcb
** Output parameters:		None ?
** Returned value:		    None ?
** Created by:				LDH
** Created Date:		    2013.02.27
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#if ((OS_CPU_HOOKS_EN > 0) && (OS_TIME_TICK_HOOK_EN > 0)) || (OS_VERSION < 283)
    void OSTimeTickHook (void)
    {
        #if OS_TMR_EN > 0
	        OSTmrCtr++;
		    if (OSTmrCtr >= (OS_TICKS_PER_SEC / OS_TMR_CFG_TICKS_PER_SEC)) {
                OSTmrCtr = 0;
                OSTmrSignal();
		    }
	    #endif
    }
#endif


/*********************************************************************************************************
  END FILE 
*********************************************************************************************************/

