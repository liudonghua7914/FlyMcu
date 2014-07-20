#define		_FLYETHERNETGOABLE_

#include "config.h"
#include "lpc17xx_lib.h"
#include "includes.h"
#include "UserType.h"

#include "tcpip.h"
#include "tcp.h"
#include "ip.h"
#include "Ethernetif.h"
#include "Etharp.h"

BYTE ip_addr[] = {192,168,8,100};
BYTE gw_addr[] = {192, 168, 1, 1};
BYTE netmask[] = { 255, 255, 0, 0};	
BYTE mac_addr[] = {MAC0,MAC1,MAC2,MAC3,MAC4,MAC5};


/***************************************************************************************************************************
**函数名称:	 	ENET_IRQHandler
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void ENET_IRQHandler(void)
{
	OSIntEnter();
	if(EMAC_IntGetStatus(EMAC_INT_RX_DONE))
	{
		
	}
	
	LPC_EMAC->IntClear = LPC_EMAC->IntStatus;
	OSIntExit();
}
/***************************************************************************************************************************
**函数名称:	 	ipcEventProcFlylyEthernet
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
static err_t client_connected(void *arg, struct tcp_pcb *pcb, err_t err)
{

}

/***************************************************************************************************************************
**函数名称:	 	LwIP_TimeOutTask
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void LwIP_TimeOutTask(void *arg)
{
	printf("\r\n LwIP_TimeOutTask %d ", OSTimeGet());
}
/***************************************************************************************************************************
**函数名称:	 	ip_input
**函数功能:	 	
				eth_hdr:
				+----------------------------------------------------------------------------------------+
				+  desMAC[6]  |  srcMAC[6]  |  type[2]( ARP / IP)  |  	数据[46-1500]   |	校验[4]      +
				+----------------------------------------------------------------------------------------+
				*注: 
					desMAC[6]: 目的MAC地址;
					srcMAC[6]: 源MAC地址;
					type[2]: 类型(IP:0X800/ARP:0X806)
					数据[46-1500]:具体数据;
					校验[4]:校验;


				etharp_hdr:
				+-----------------------------------------------------------------------------------------------------+
				+ hwhdr[2] | porotl[2] | hwlen[1] | porotllen[1] | op[2] | srcMAC[6] | srcIP[4] |desMAC[6] | desIP[4] +
				+-----------------------------------------------------------------------------------------------------+
				*注: 
					hwhdr[2]:硬件类型字段-表示硬件地址的类型,它的值为1即表示以太网MAC地址,长度为6个字节;
					porotl[2]:协议类型字段表示要映射的协议地址类型,它的值为0X800即表示要映射的IP地址;
					hwlen[1]: 硬件地址长度,对于以太网ARP请求或者应答来说,他的值为6;
					porotllen[1]:协议地址长度,对于以太网ARP请求或者应答来说,他的值为4;
					op[2]:操作类型 ARP请求=1,ARP应答=2,RARP请求=3,RARP=4;
					srcMAC[6]:发送端MAC地址;
					srcIP[4]: 发送端IP地址;
					desMAC[6]:目的端MAC地址
					desIP[4]: 目的端IP地址;
				ip_hdr:
				+---------------------------------------------------------------------------------------------------------------------------------------------------------------------+
				+ v(bit4) | hdrlen(bit4) | TOS[1] | len[2] | flag0[2] | flag1(bit3) | offset(bit13) | ttl[1] | porotl[1] |  hdrchecsum[2] | srcIP[4] | desIP[4] | 选项[...] | data[N] +																								  +
				+---------------------------------------------------------------------------------------------------------------------------------------------------------------------+
				*注: 
				v(bit4): 版本号,目前的版本号是4即IPv4;
				hdrlen(bit4): 首部长度-指首部站32Bit字的数目,包括可选项,由于它是个4bit因此首部最长60字节;
				TOS[1]:包括一个3bit的优先字段,4bit TOS字段和1 bit未使用但必须为0,如果所有4bit都为0,即一般服务;
				len[2]:整个IP数据报的长度以字节为单位;
				flag0[2]:指唯一标示主机发送的每一份数据报,通常每发送一份报文值就会加1;
				flag1(bit3): 用于在IP数据包分片时使用;
				offset(bit13):  用于在IP数据包分片时使用;
				ttl[1]:描述该IP数据包最多能被转发多少次,每经过一次转发,该值就会减1,当该值为0时,一个ICMP报文会被返回至源主机;
				porotl[1]: 协议字段描述该IP数据包是来自上层哪个协议,改值为1表示为ICMP,2=IGMP,6=TCP,17=UDP;
				hdrchecsum[2]:首部校验和只针对IP首部做校验,它并不关心其内部数据在传输过程中出错与否,对于数据校验是上层协议负责的;
				srcIP[4]:源IP地址;
				desIP[4]:目标IP地址;
				选项[...]:选项,可有可无;
				data[N]:数据data;
				
				icmp_hdr:
				+--------------------------------------------------------------------------------+
				+ type[1] | code[1] | checksum[2] | 	others[N](不同的类型和代码有不同的内容)  |
				+--------------------------------------------------------------------------------+
				
				
**入口参数:
**返回参数:
***************************************************************************************************************************/
void ipcFlyEthernetInit(void)
{
	struct ip_addr ip_computer;
	memset(&flyEhternetInfo,0,sizeof(flyEhternetInfo));
	tcpip_init(NULL,NULL);
	
	IP4_ADDR(&flyEhternetInfo.ipaddr,  192, 168, 8, 100);
	IP4_ADDR(&flyEhternetInfo.netmask, 255, 255, 0, 0);
	IP4_ADDR(&flyEhternetInfo.gw, 192, 168, 1, 1);
	
	
	netif_add(&flyEhternetInfo.netif, &flyEhternetInfo.ipaddr, &flyEhternetInfo.netmask, &flyEhternetInfo.gw, NULL, ethernetif_init, ethernet_input);
	netif_set_default(&flyEhternetInfo.netif);
	netif_set_up(&flyEhternetInfo.netif);
	
		
	
	flyEhternetInfo.pUpdpcb = udp_new();
	if(NULL == flyEhternetInfo.pUpdpcb)
	{
		printf("\r\n udp_new fail");
	}
	
	if(ERR_OK != udp_bind(flyEhternetInfo.pUpdpcb,IP_ADDR_ANY,80))
	{
		printf("\r\n udp_bind fail");
	}
	
	if(ERR_OK != udp_connect(flyEhternetInfo.pUpdpcb,IP_ADDR_ANY,80))
	{
		printf("\r\n udp_connect fail");
	}
	
	flyEhternetInfo.pTcp = tcp_new();
	if(NULL == flyEhternetInfo.pTcp)
	{
		printf("\r\n tcp_new fail");
	}
	
	if(ERR_OK != tcp_bind(flyEhternetInfo.pTcp,IP_ADDR_ANY,80))//IP_ADDR_ANY
	{
		printf("\r\n tcp_bind fail");
	}
	
	if(ERR_OK != tcp_connect(flyEhternetInfo.pTcp,&ip_computer,80,client_connected))
	{
		printf("\r\n tcp_connect fail");
	}
	
	
	
	FlylyEthernetCreate();
	printf("\r\n ipcFlyEthernetInit OK");	
}
/***************************************************************************************************************************
**函数名称:	 	ipcEventProcFlylyEthernet
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void ipcEventProcFlylyEthernet(ULONG enumWhatEvent,ULONG lPara,BYTE *p,uint8_t length)
{
	BOOL bRes = TRUE;
	switch(enumWhatEvent)
	{
		case EVENT_GLOBAL_MODULE_INIT:		ipcFlyEthernetInit();				
											break;
		
		default:							break;
	}
	
	if(bRes)
	{
		ipcClearEvent(enumWhatEvent);
	}
}

#if 1
/***************************************************************************************************************************
**函数名称:	 	FlyEthernetTask
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void FlyEthernetTask(void *arg)
{
	uint32 Timerl = OSTimeGet();
	while(1)
	{
		OSTimeDly(OS_TICKS_PER_SEC/50);  
		ethernetif_input(&flyEhternetInfo.netif);
		
		if((OSTimeGet() - Timerl) > OS_TICKS_PER_SEC * 5)
		{
			Timerl = OSTimeGet();					
			if(ERR_OK != tcpip_timeout(1000, LwIP_TimeOutTask, NULL))
			{
				printf("\r\n tcpip_timeout fail");
			}	
		}		
	}
}
/***************************************************************************************************************************
**函数名称:	 	FlylyEthernetCreate
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void FlylyEthernetCreate(void)
{
	INT8U Res;
	Res = OSTaskCreate(FlyEthernetTask, 								//执行函数
				 NULL,	  												//带入的参数
				 &GstkFlyEthernet[FLYETHERNET_TASK_START_STK_SIZE-1],	//堆栈由高地址往底地址增长
				 (INT8U)PRIO_FLYEHTERNET								//任务优先级
				 );
	if(OS_NO_ERR != Res)
	{
		printf("\r\n Res = %d",Res);
	}
}
#endif