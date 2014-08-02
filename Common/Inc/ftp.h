#ifndef 	__FTP_H__
#define		__FTP_H__

#include "def.h"
#include "tcpip.h"
#include "tcp.h"
#include "ip.h"

typedef struct sfifo_t
{
	uint8_t xxx;
} sfifo_t;


struct ftpd_datastate 
{
	uint8_t xxx;
};

struct ftpd_msgstate 
{
	uint8_t xxx;
};

struct ftpd_command 
{
	char *cmd;
	void (*func) (const char *arg, struct tcp_pcb *pcb, struct ftpd_msgstate *fsm);
};




























#endif
