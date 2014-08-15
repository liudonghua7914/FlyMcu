#ifndef 	__FTP_H__
#define		__FTP_H__

#include "def.h"
#include "tcpip.h"
#include "tcp.h"
#include "ip.h"

typedef void (* _p_ftp_write_data)(struct netconn *ftpconn,char *p,UINT len);

enum ftpd_state_e 
{
	FTPD_USER = 0,
	FTPD_PASS,
	FTPD_IDLE,
	FTPD_NLST,
	FTPD_LIST,
	FTPD_RETR,
	FTPD_RNFR,
	FTPD_STOR,
	FTPD_QUIT
};

enum ftpnum
{
	N0 = 0,
	N1,
	N2,
	N3,
	N4,
	N5,
	N6,
	N7,
	N8,
	N9,
	N10,
	N11,
	N12,
	N13,
	N14,
	N15,
	N16,
	N17,
	N18,
	N19,
	N20,
	N21,
	N22,
	N23,
//	N24,
	MAX,
};

typedef struct sfifo_t
{
	uint8_t xxx;
} sfifo_t;


struct ftpd_datastate 
{
	BOOL  bConnect;
	FS_DIR *dirp;
};

struct ftpd_msgstate 
{
	UINT16 len;
	struct ip_addr dataip;
	UINT16 dataport;
	char curPatch[64];
	char curname[32];
	BYTE patchLen;
	enum ftpd_state_e state;
	struct ftpd_datastate *datafs;

	FTP_INFO *pftpinfo;
	FTP_FUNC *pftpfunc;
};

struct ftpd_command 
{
	BYTE id;
	char *cmd;
	void (*func) (const char *arg,struct ftpd_msgstate *fsm);
};




























#endif
