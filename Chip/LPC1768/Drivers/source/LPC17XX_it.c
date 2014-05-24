
#include"Config.h"	 //changed by LDH
#include "lpc17xx_lib.h"

#if(DEVELOPMENT	== MTK_DEVELOPMENT)
	#include "fly_PinCfg_lpc1752_MTK.h"
#elif(DEVELOPMENT	== QUALCOMM_DEVELOPMENT)
	
#endif

#ifdef _IT

void EINT3_IRQHandler(void)
{
	if(GPIO_GetIntStatus(ACC_IN_PORT, ACC_IN_PIN, 1))
	{
	  	GPIO_ClearInt(ACC_IN_PORT,(1<<ACC_IN_PIN));
	}

	if(GPIO_GetIntStatus(LCAN_RX_PORT, LCAN_RX_PIN, 1))
	{
	  	GPIO_ClearInt(LCAN_RX_PORT,(1<<LCAN_RX_PIN));
	}

}
#endif

