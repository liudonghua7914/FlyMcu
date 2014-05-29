/****************************************Copyright (c)****************************************************
**                               Guangzou FlyAudio Development Co.LTD.
**                                     
**                                 http://www.FlyAudio.com
**
**------------------------------------File Info---------------------------------------------------------
** File name: 			    main.c
** Last modified Date:     	2013.02.27
** Last Version: 		    1.0
** Descriptions: 	        The assembly functions that a uC/OS-II port requires
**                         
**-------------------------------------------------------------------------------------------------------
** Created By:       	    LDH
** Created date:   	   		2013.02.27
** Version:			    	1.0
** Descriptions: 		    
**
**-------------------------------------------------------------------------------------------------------
** Modified by:            
** Modified date:          2013.02.27
** Version:                1.0
** Description:            
**
*********************************************************************************************************/

#define 	_GLOBAL_INFO_GLOABL_
#include "config.h"
#include "includes.h"
#include "lpc17xx_lib.h"
#include "fs_include.h"
#include "UserType.h"



/*----------------------------------------------------------------------------------------------------------------------------------------------------------

������ϵͳ:	��Ϊ3��,����������û�����,�м��ΪOS,��������Ǹ�Ӳ����صĲ���lpc17xx;
			����ϵͳ��ÿ���㶼����һ��һ��ͷ�ļ�(�ֱ�Ϊ:UserType.h,includes.h,lpc17xx_lib.h)
			���������������һЩ���ļ�,���㴦��;  config.hΪϵͳ�������ļ�

User:  		 main.c (UserType.h)  UserType.h����:	
													
------------------------------------------------------------------------------------------------------------------------------------------------------------
Ӳ�������: interface.c	��Ӳ�������һ��
------------------------------------------------------------------------------------------------------------------------------------------------------------
OS:			 OS_CORE.C (includes.h) includes.h����:	os_cfg.h,os_cpu.h,ucos_ii.h	
------------------------------------------------------------------------------------------------------------------------------------------------------------
LPC17xx:	 (lpc17xx_gpio.h lpc17xx_lib.h)
------------------------------------------------------------------------------------------------------------------------------------------------------------


1. �����ж����ȼ��ǵ���...........
   1.1 	�������ж����ȼ���ʱ������ȥ�����ж����ȼ��Ĵ���NVIC->IP[240];��һ��MCU��̫������240���ж�,�����������
   		��֪�����MCU�ж��ٸ��ж��أ����ǿ���ȥ��һ���Ĵ���SCnSCB->ICTR��ַ��0xE000E004	,����ʵ��ó�bit[4:0] = 1;
   		ICTR:

	   	bit
	   	4:0  INTLINESUM  R  -  �ж�������������� 32Ϊ���ȣ���
	   												0=1��32 
													1=33�� 64 
													2=65�� 96  

	1.2 ���ж����ȼ����и���ռ���ȼ��������ȼ��ĸ���:
			����:CM3���� 256�����ȼ���λ�ֳɸߵ����Σ��ֱ��Ϊ��ռ���ȼ��������ȼ�,NVIC����һ���Ĵ����ǡ�Ӧ�ó���
		�жϼ���λ���ƼĴ�����(AIRCR ��ַ��0xE000ED00)����������һ��λ����Ϊ�����ȼ��顱(bit[10:8]),
		��λ�ε�ֵ��ÿһ�����ȼ������õ��쳣����Ӱ�졪���������ȼ���Ϊ2��λ��:MSB���ڵ�λ��(��ߵ�)��Ӧ��ռ���ȼ�,
																	|
			Bit7	Bit6	Bit5	Bit4	Bit3	Bit2	Bit1	| Bit0
			  X		  X		  X		 X		 X		 X		  X		|  X
																	|
			����λ��  	������ռ���ȼ���λ��  	���������ȼ���λ��	
				0			[7:1]  					[0:0] 
				1  			[7:2]  					[1:0] 
				2  			[7:3]  					[2:0] 
				3  			[7:4]  					[3:0] 
				4  			[7:5]  					[4:0] 
				5  			[7:6]  					[5:0] 
				6  			[7:7]  					[6:0] 
				7  			��  					[7:0](����λ)
		
			���������֪��оƬ���ڷ�����������ǿ���ȥ��SCB->AIRCR�üĴ�����(bit[10:8])���ߵ�NVIC_GetPriorityGrouping()����,
		����ʵ��ó�(bit[10:8] = 000������оƬ�ķ����ǵ�0��,������ֻ�õ�ǰ���5λ����ô��������ʹ�õ����ȼ�������ռ���ȼ�;
			�����ǿ����޸ķ������ô�����ǿ��ԵΣ��������ȼ��ķ��鶼ҪԤ�Ⱦ���������֤,�����ڿ�����ʼ��ʱһ���Ե����ú�,
		�Ժ����Ҳ��������,ֻ���ھ�����Ҫ�Ҿ����а���ʱ,��С�ĵظ���,����Ҫ���������ܳ�ֵĲ���,����,���ȼ������ڵļĴ���AIRCR
		Ҳ�������ǡ�һ�γ��͡�,ֻ����Ҫ�ֹ�������λʱ��д������Ӧ��λ.



2.  ����HardFault_Handler�������Щ����.......
	2.1	ÿ�ε��Գ����ʱ�����HardFault_Handler�����Ƿǳ�ͷ�۵�һ����,�����������,ͷ��....ͷ��.....;
		û�취,ͷ��Ҳ�ý��,Ӳ��ͷƤ����Cortex-M3Ȩ��ָ�ϡ�,д���治��,����CM3�ں˵�����,�ѵð�,��ש���о�����,������HardFault_Handler����ʱ,
		����,����ȥlook��Ӳfault״̬�Ĵ���,��ַ��0xE000ED2C ���������߷����ʱ����Կ��������ַ�е�����:
			
			Ӳfault״̬�Ĵ���:		

			λ��		����			����		��λֵ	  				����
			bit31		DEBUGEVT		 R/W		   0			Ӳ fault������¼�������
			bit30		FORCED			 R/W		   0			Ӳ fault������ fault���洢������ fault�����÷� fault�ϷõĽ�� 
			bit29~2		  -				  -			   -					 -
			bit1		 VECTB			 R/W		   0			Ӳ fault����ȡ����ʱ������
			bit0			-			  -			   -					 -

			��������Ҫ��ע��bit30,���������и�fault�ϷõĶ���.....�Ǻη���ʥ.....,ò����fault������ fault���洢������ fault�����÷� fault���ִ���Ľ��...
			���������ֲ���ȷ���������ĸ�fault��ɵ�HardFault_Handler,��......ͷ��ʹ��.....��������ʱ�������ֵÿ�������fault��״̬�Ĵ�����...

			������CM3�е� fault״̬�Ĵ�����: 

			0xE000_ED28  MMSR  MemManage fault״̬�Ĵ��� 	1�ֽ� 
			0xE000_ED29  BFSR  ���� fault״̬�Ĵ���  		1�ֽ�
			0xE000_ED2A  UFSR  �÷� fault״̬�Ĵ���  		2�ֽ�
			0xE000_ED2C  HFSR  Ӳ fault״̬�Ĵ���  			4�ֽ�
			0xE000_ED30  DFSR  ���� fault״̬�Ĵ���  		4�ֽ�  
			0xE000_ED3C  AFSR  ���� fault״̬�Ĵ���  		4�ֽ�

			������Ҫ����BFSR��MMSR:(pdf->P320)

			 MMSR:

			 λ��		����			����		��λֵ	  				����			���ܵ�ԭ��
			  7 	  MMARVALID  	      -  		  0  			=1ʱ��ʾ MMAR��Ч 
			 6:5 		  -				  -			  -						 -
			  4		   MSTKERR 			 R/W		  0				 ��ջʱ�������� 		1.	��ջָ���ֵ���ƻ���		2.	��ջ���׹����Ѿ����� MPU������ region��Χ  
			  3		  MUNSTKERR			 R/W		  0				 ��ջʱ��������			1.  �쳣���������ƻ��˶�ջָ��;	2.  �쳣�������̸����� MPU���� 
			  2			  -	 			  -			  -					  -
			  1		  DACCVIOL			 R/W		  0				 ���ݷ���Υ��			�ڴ���ʱ���Υ��������MPU�������õ����֡��������û�Ӧ�ó�����ͼ������Ȩ�� region���� 
			  0		  IACCVIOL			 R/W		  0				 ȡָ����Υ��

			  BFSR��

			  λ��		����			����		��λֵ	  			����				���ܵ�ԭ��
			  7 	  BFARVALID  	      -  		  0  			=1ʱ��ʾ MMAR��Ч 
			 6:5 		  -				  -			  -						 -
			  4		   STKERR 			 R/W		  0				 ��ջʱ�������� 		�Զ���ջ�ڼ���� : 1.  ��ջָ���ֵ���ƻ�   2.  ��ջ����̫�󣬵�����δ����洢�������� 3.  PSPδ����ʼ����ʹ�� 
			  3		  UNSTKERR			 R/W		  0				 ��ջʱ��������			�Զ���ջ�ڼ���������û�з����� STKERR��������ܵľ������쳣�����ڼ�� SP��ֵ�ƻ��� 
			  2		 IMPRECISERR 		 R/W		  0				 ����ȷ�����ݷ���Υ��
			  1		  DACCVIOL			 R/W		  0				 ��ȷ�����ݷ���Υ�� 
			  0		  IACCVIOL			 R/W		  0				 ȡָʱ�ķ���Υ�� 


			 �������� fault �ʹ洢������ fault,��ʱ���ܾ�ȷ��λ����ָ��ĵ�ַ,��MMAVALID/BFARVALID λ����λʱ���Ǿ�ȷ fault���洢������ fault�ĵ�ַ�洢�� MMAR��
			 ���� fault�ĵ�ַ��洢�� BFAR�У�������ʵ���ϣ�MMAR�� BFAR��ʵ��ͬһ���Ĵ���,���ͬһʱ��ֻ����һ������������Ϊͬһʱ��ֻ�ܳ���һ��fault.

			 CM3 �е� fault��ַ�Ĵ�����

			 0xE000_ED34  MMAR  MemManage fault��ַ�Ĵ��� 		4�ֽ�
			 0xE000_ED38  BFAR  ���� fault��ַ�Ĵ���  			4�ֽ�

			 ˵��:	��� MMARVALID �� BFARVALID Ϊ 1������ṩ����ʱ�ĵ�ַ,	��Ҫע����ǣ���MARVALID/BFAVRALID �������fault ��ַ�Ĵ����е�ֵ���ܱ�����,
			 ��ˣ������ȶ�BFAR/MMAR���ٶ� BFARVALID/MMARVALID.�������Ϊ�㣬���������ĵ�ֵַ��ͨ�׵Ľ���� MMARVALID �� BFARVALID Ϊ 1,MMAR/BFAR������
			 ��ʱ����ʱ�ĵ�ַ;ע��ǰ����MMARVALID �� BFARVALID Ϊ 1;

			 
			2.2 �ϱ� fault״̬�Ĵ���
			 Fault��������������Ĺ��������ϱ� fault״̬�Ĵ�����ֵ;
			 ��λ��ջPC������:
			 	   		
			 					  	   LR[bit2] = 0 / 1	 ?
									   			|
			 									|
												|
										 _______|_______
										|				|
										| = 0		1 =	|
										|				|
									  ʹ��MSP		 ʹ��PSP
										|				|
										|				|
										|				|
										 ������������    ������������
										    ��SP-0X24��
											��ȡ��ջPCֵ
											 ��λPC

			�ܽ�:������HardFault_Handlerʱ:
				
				 1.�鿴HFSR  Ӳ fault״̬�Ĵ���,��bit30�Ƿ�Ϊ1,�����1,�򿴵ڶ���,�������,������.......��
				 2.�鿴MMSR��BFSR��MMARVALID �� BFARVALID λ�Ƿ�Ϊ1,�����1,�����ֱ�Ӷ�ȡMMAR/BFAR�е�ֵ,ȷ�������쳣ʱ�ĵ�ַ;���Ϊ0,�򿴵�3��;
				 3.�鿴MMSR��BFSR�Ĵ���������λ���Ƿ�Ϊ1,���� BFSR��STKERRΪ1,���Զ���ջ�ǳ��ִ���,��ʱ�����Ǿ�Ҫ�鿴��ǰʹ�õ���MSP����PSP,���
				   �����Ƿ��ڽ����жϷ�����ǰ��û�н�MSP����PSPָ��һ���Ϸ��ĵ�ַ(�ڴ�)�ռ�,���Ը����ֲ���˵�Ŀ���ԭ������Լ��Ĵ���������...����
				 4.Ҳ����ͨ��2.2��˵�ķ����ҳ��쳣ʱPCָ���ֵ,�����ַ�����һ��ÿ�ζ��е��ر������MSP/PSPָ��һ������ռ�ʱ���޷�ȷ����....
----------------------------------------------------------------------------------------------------------------------------------------------------------*/
















/***********************************************************************
**��������:	  main
**��������:
**��ڲ���:
**���ز���:
***********************************************************************/
int main(void)
{
	IntDisAll();
	chipInit();
	OSInit();
	if(FS_Init() < 0)
	{
		printf("\r\n FS_Init Fail");
	}
	TaskInit();
   	OSStart();
	return 0;
}




