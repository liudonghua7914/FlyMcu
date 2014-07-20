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
**��������:	 	ENET_IRQHandler
**��������:	 	
**��ڲ���:
**���ز���:
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
**��������:	 	ipcEventProcFlylyEthernet
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
static err_t client_connected(void *arg, struct tcp_pcb *pcb, err_t err)
{

}

/***************************************************************************************************************************
**��������:	 	LwIP_TimeOutTask
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
void LwIP_TimeOutTask(void *arg)
{
	printf("\r\n LwIP_TimeOutTask %d ", OSTimeGet());
}
/***************************************************************************************************************************
**��������:	 	ip_input
**��������:	 	
				eth_hdr:
				+----------------------------------------------------------------------------------------+
				+  desMAC[6]  |  srcMAC[6]  |  type[2]( ARP / IP)  |  	����[46-1500]   |	У��[4]      +
				+----------------------------------------------------------------------------------------+
				*ע: 
					desMAC[6]: Ŀ��MAC��ַ;
					srcMAC[6]: ԴMAC��ַ;
					type[2]: ����(IP:0X800/ARP:0X806)
					����[46-1500]:��������;
					У��[4]:У��;


				etharp_hdr:
				+-----------------------------------------------------------------------------------------------------+
				+ hwhdr[2] | porotl[2] | hwlen[1] | porotllen[1] | op[2] | srcMAC[6] | srcIP[4] |desMAC[6] | desIP[4] +
				+-----------------------------------------------------------------------------------------------------+
				*ע: 
					hwhdr[2]:Ӳ�������ֶ�-��ʾӲ����ַ������,����ֵΪ1����ʾ��̫��MAC��ַ,����Ϊ6���ֽ�;
					porotl[2]:Э�������ֶα�ʾҪӳ���Э���ַ����,����ֵΪ0X800����ʾҪӳ���IP��ַ;
					hwlen[1]: Ӳ����ַ����,������̫��ARP�������Ӧ����˵,����ֵΪ6;
					porotllen[1]:Э���ַ����,������̫��ARP�������Ӧ����˵,����ֵΪ4;
					op[2]:�������� ARP����=1,ARPӦ��=2,RARP����=3,RARP=4;
					srcMAC[6]:���Ͷ�MAC��ַ;
					srcIP[4]: ���Ͷ�IP��ַ;
					desMAC[6]:Ŀ�Ķ�MAC��ַ
					desIP[4]: Ŀ�Ķ�IP��ַ;
				ip_hdr:
				+---------------------------------------------------------------------------------------------------------------------------------------------------------------------+
				+ v(bit4) | hdrlen(bit4) | TOS[1] | len[2] | flag0[2] | flag1(bit3) | offset(bit13) | ttl[1] | porotl[1] |  hdrchecsum[2] | srcIP[4] | desIP[4] | ѡ��[...] | data[N] +																								  +
				+---------------------------------------------------------------------------------------------------------------------------------------------------------------------+
				*ע: 
				v(bit4): �汾��,Ŀǰ�İ汾����4��IPv4;
				hdrlen(bit4): �ײ�����-ָ�ײ�վ32Bit�ֵ���Ŀ,������ѡ��,�������Ǹ�4bit����ײ��60�ֽ�;
				TOS[1]:����һ��3bit�������ֶ�,4bit TOS�ֶκ�1 bitδʹ�õ�����Ϊ0,�������4bit��Ϊ0,��һ�����;
				len[2]:����IP���ݱ��ĳ������ֽ�Ϊ��λ;
				flag0[2]:ָΨһ��ʾ�������͵�ÿһ�����ݱ�,ͨ��ÿ����һ�ݱ���ֵ�ͻ��1;
				flag1(bit3): ������IP���ݰ���Ƭʱʹ��;
				offset(bit13):  ������IP���ݰ���Ƭʱʹ��;
				ttl[1]:������IP���ݰ�����ܱ�ת�����ٴ�,ÿ����һ��ת��,��ֵ�ͻ��1,����ֵΪ0ʱ,һ��ICMP���Ļᱻ������Դ����;
				porotl[1]: Э���ֶ�������IP���ݰ��������ϲ��ĸ�Э��,��ֵΪ1��ʾΪICMP,2=IGMP,6=TCP,17=UDP;
				hdrchecsum[2]:�ײ�У���ֻ���IP�ײ���У��,�������������ڲ������ڴ�������г������,��������У�����ϲ�Э�鸺���;
				srcIP[4]:ԴIP��ַ;
				desIP[4]:Ŀ��IP��ַ;
				ѡ��[...]:ѡ��,���п���;
				data[N]:����data;
				
				icmp_hdr:
				+--------------------------------------------------------------------------------+
				+ type[1] | code[1] | checksum[2] | 	others[N](��ͬ�����ͺʹ����в�ͬ������)  |
				+--------------------------------------------------------------------------------+
				
				
**��ڲ���:
**���ز���:
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
**��������:	 	ipcEventProcFlylyEthernet
**��������:	 	
**��ڲ���:
**���ز���:
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
**��������:	 	FlyEthernetTask
**��������:	 	
**��ڲ���:
**���ز���:
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
**��������:	 	FlylyEthernetCreate
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
void FlylyEthernetCreate(void)
{
	INT8U Res;
	Res = OSTaskCreate(FlyEthernetTask, 								//ִ�к���
				 NULL,	  												//����Ĳ���
				 &GstkFlyEthernet[FLYETHERNET_TASK_START_STK_SIZE-1],	//��ջ�ɸߵ�ַ���׵�ַ����
				 (INT8U)PRIO_FLYEHTERNET								//�������ȼ�
				 );
	if(OS_NO_ERR != Res)
	{
		printf("\r\n Res = %d",Res);
	}
}
#endif