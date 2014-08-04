#ifndef 	__FTP_H__
#define		__FTP_H__

#include "def.h"
#include "tcpip.h"
#include "tcp.h"
#include "ip.h"

typedef void (* _p_ftp_write_data)(char *p,UINT len);

enum ftpd_state_e 
{
	FTPD_USER,
	FTPD_PASS,
	FTPD_IDLE,
	FTPD_NLST,
	FTPD_LIST,
	FTPD_RETR,
	FTPD_RNFR,
	FTPD_STOR,
	FTPD_QUIT
};

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
	enum ftpd_state_e state;
	_p_ftp_write_data ftp_write_data;
};

struct ftpd_command 
{
	char *cmd;
	void (*func) (const char *arg, struct tcp_pcb *pcb, struct ftpd_msgstate *fsm);
};




























#endif
