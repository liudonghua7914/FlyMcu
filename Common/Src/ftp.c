#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>



#include "lwip/stats.h"
#include "ftp.h"

#include "def.h"
#include "tcpip.h"
#include "tcp.h"
#include "ip.h"


/*
	��׼FTP��Ϣ:	
	
	
	

��Ӧ����						����˵��
  110						���ļ�ָʾ���ϵ��������
  120						��������������ʱ��(������)
  125						����������,��ʼ����
  150						������
  200						�ɹ�
  202						����û��ִ��
  211						ϵͳ״̬�ظ�
  212						Ŀ¼״̬�ظ�
  213						�ļ�״̬�ظ�
  214						������Ϣ�ظ�
  215						ϵͳ���ͻظ�
  220					    �������
  221						�˳�����
  225						����������
  226						�ر���������,�����ļ������ɹ�
  227						������������
  230						��½Ӣ����
  250						�ļ���Ϊ���
  257						�ļ�·��������
  331						Ҫ������
  332						Ҫ���˺�
  350						�ļ���Ϊ��ͣ
  421						����ر�
  425						�޷�����������
  426						��������
  450						�ļ�������
  451						�������ش���
  452						���̿ռ䲻��
  500						��Ч����
  501						�������
  502						����û��ִ��
  503						����ָ������
  504						��Ч�������
  530						δ��¼����
  532						�洢�ļ���Ҫ�˺�
  550						�ļ�������
  551						��֪��ҳ������
552						�����洢����
  553						�ļ���������
*/

#define msg110 "110 MARK %s = %s."
/*
         110 Restart marker reply.
             In this case, the text is exact and not left to the
             particular implementation; it must read:
                  MARK yyyy = mmmm
             Where yyyy is User-process data stream marker, and mmmm
             server's equivalent marker (note the spaces between markers
             and "=").
*/
#define msg120 "120 Service ready in nnn minutes."
#define msg125 "125 Data connection already open; transfer starting."
#define msg150 "150 File status okay; about to open data connection."
#define msg150recv "150 Opening BINARY mode data connection for %s (%i bytes)."
#define msg150stor "150 Opening BINARY mode data connection for %s."
#define msg200 "200 Command okay."
#define msg202 "202 Command not implemented, superfluous at this site."
#define msg211 "211 System status, or system help reply."
#define msg212 "212 Directory status."
#define msg213 "213 File status."
#define msg214 "214 %s."
#define msg214SYST "214 %s system type."
#define msg220 "220 lwIP FTP Server ready."
#define msg221 "221 Goodbye."
#define msg225 "225 Data connection open; no transfer in progress."
#define msg226 "226 Closing data connection."
#define msg227 "227 Entering Passive Mode (%i,%i,%i,%i,%i,%i)."
#define msg230 "230 User logged in, proceed."
#define msg250 "250 Requested file action okay, completed."
#define msg257PWD "257 \"%s\" is current directory."
#define msg257 "257 \"%s\" created."
#define msg331 "331 User name okay, need password."
#define msg332 "332 Need account for login."
#define msg350 "350 Requested file action pending further information."
#define msg421 "421 Service not available, closing control connection."
#define msg425 "425 Can't open data connection."
#define msg426 "426 Connection closed; transfer aborted."
#define msg450 "450 Requested file action not taken."
#define msg451 "451 Requested action aborted: local error in processing."
#define msg452 "452 Requested action not taken."
#define msg500 "500 Syntax error, command unrecognized."
#define msg501 "501 Syntax error in parameters or arguments."
#define msg502 "502 Command not implemented."
#define msg503 "503 Bad sequence of commands."
#define msg504 "504 Command not implemented for that parameter."
#define msg530 "530 Not logged in."
#define msg532 "532 Need account for storing files."
#define msg550 "550 Requested action not taken."
#define msg551 "551 Requested action aborted: page type unknown."
#define msg552 "552 Requested file action aborted."
#define msg553 "553 Requested action not taken."

static int msgcount = 0;
static struct ftpd_msgstate msgstate;
static struct ftpd_msgstate *fsm;
static struct ftpd_datastate datastate;
static char buffer[128]; 
static char *pDirPatch = NULL;
static const char *month_table[12] = 
{
	"Jan",
	"Feb",
	"Mar",
	"Apr",
	"May",
	"Jun",
	"Jul",
	"Aug",
	"Sep",
	"Oct",
	"Nov",
	"Dez"
};

/***************************************************************************************************************************
**��������:	 	getFTPMsgState
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
BYTE getFTPMsgState(void)
{
	return fsm->state;
}
/***************************************************************************************************************************
**��������:	 	setFTPMsgState
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
void setFTPMsgState(BYTE state)
{
	fsm->state = state;
}
/***************************************************************************************************************************
**��������:	 	sfifo_init
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
static int sfifo_init(sfifo_t *f,int size)
{
	return 0;
}
/***************************************************************************************************************************
**��������:	 	sfifo_close
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
static void sfifo_close(sfifo_t *f)
{

}
/***************************************************************************************************************************
**��������:	 	sfifo_flush
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
static void sfifo_flush(sfifo_t *f)
{
	
}
/***************************************************************************************************************************
**��������:	 	sfifo_write
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
static int sfifo_write(sfifo_t *f)
{
	return 0;
}
/***************************************************************************************************************************
**��������:	 	sfifo_read
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
static int sfifo_read(sfifo_t *f,void *buf,int len)
{
	return 0;
}

/***************************************************************************************************************************
**��������:	 	send_msg
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
static void send_msg(char *msg, ...)
{
	va_list arg;
	va_start(arg, msg);
	memset(buffer,'\0',sizeof(buffer));
	vsprintf(buffer, msg, arg);
	va_end(arg);
	strcat(buffer, "\r\n");
	fsm->pftpinfo->ctrOrMsg = eCtr;
	if(fsm->pftpfunc->p_ftp_writeData)
	{
		fsm->pftpfunc->p_ftp_writeData(fsm->pftpinfo,buffer,strlen(buffer));
	}
}
/***************************************************************************************************************************
**��������:	 	send_data
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
static void send_data(char *p,UINT len)
{
	LIBMCU_DEBUG(FTP_DEBUG,("\r\n send_data len %d ",len));
	memset(buffer,'\0',sizeof(buffer));
	memcpy(buffer,p,len);
	strcat(buffer, "\r\n");
	fsm->pftpinfo->ctrOrMsg = eMsg;
	if(fsm->pftpfunc->p_ftp_writeData)
	{
		fsm->pftpfunc->p_ftp_writeData(fsm->pftpinfo,buffer,strlen(buffer));
	}
}


/***************************************************************************************************************************
**��������:	 	ftpd_datarecv
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
static err_t ftpd_datarecv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
	return 0;
}
/***************************************************************************************************************************
**��������:	 	send_next_directory
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
static void send_next_directory(struct ftpd_datastate *fsd, struct tcp_pcb *pcb, int shortlist)
{
#if 0
	struct FS_DIRENT *direntp;
	LIBMCU_DEBUG(FTP_DEBUG,("\r\n FS_OpenDir: %s ",pDirPatch));
	
	if(!fsd->dirp)
	{
		fsd->dirp = FS_OpenDir(pDirPatch);
	}	
	if(!fsd->dirp)
	{
		send_msg(msg425);
		send_msg(msg426);
		send_msg(msg451);
		LIBMCU_DEBUG(FTP_DEBUG,("\r\n FS_OpenDir fail "));
	}
	
	if(fsd->dirp)
	{
		do
		{
			direntp = FS_ReadDir(fsd->dirp);
			if(direntp)
			{
				LIBMCU_DEBUG(FTP_DEBUG,("\r\n d_name %s ",direntp->d_name));
				send_data(direntp->d_name,strlen(direntp->d_name));
				memset(direntp->d_name,'\0',32);
			}
			
		}while(direntp);
	}
#else
	msgcount++;
	if(msgcount < 10)
	{
		send_data("-rwxrw-r-- 1 user group 1024 NOV 12 17:57 flyaudio.txt",strlen("-rwxrw-r-- 1 user group 1024 NOV 12 17:57 flyaudio.txt"));
		send_data("drw-rw-rw- 1 user group 0 NOV 12 17:57 flyaudio",strlen("drw-rw-rw- 1 user group 0 NOV 12 17:57 flyaudiot"));
	}
	else if(msgcount == 11)
	{
		send_msg(msg226);
		send_msg(msg250);
	}		
#endif	
}
/***************************************************************************************************************************
**��������:	 	open_dataconnection
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
static int open_dataconnection(struct ftpd_msgstate *fsm)
{
	int err = -1;
	
	fsm->datafs = &datastate;
	memset(fsm->datafs,0,sizeof(struct ftpd_datastate));
	fsm->pftpinfo->ctrOrMsg = eMsg;
	fsm->pftpfunc->p_ftp_new(fsm->pftpinfo);
	if(NULL == fsm->pftpinfo->ftpRAWDataMsg)
	{
		LIBMCU_DEBUG(FTP_DEBUG,("\r\n p_ftp_new fail "));
	}
	
	fsm->pftpfunc->p_ftp_bind(fsm->pftpinfo,20);
	fsm->pftpfunc->p_ftp_arg(fsm->pftpinfo,(void *)fsm->datafs);
	fsm->pftpfunc->p_ftp_connect(fsm->pftpinfo,fsm->dataip,fsm->dataport);
	
	return 0;
}
/***************************************************************************************************************************
**��������:	 	cmd_list_common
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
static void cmd_list_common(char *p,struct ftpd_msgstate *fsm)
{
	if(!p)
	{
		return;
	}
	
	if(!memcmp(p,"root",strlen("root")))
	{
		memcpy(p,"mmc:",sizeof("mmc:"));
	}
	else
	{
		
	}
		
	
	if(0 != open_dataconnection(fsm))
	{
		send_msg(msg451);
		LIBMCU_DEBUG(FTP_DEBUG,("\r\n open_dataconnection fail "));
	}
	fsm->state = FTPD_LIST;
#if 0	

#endif	
}
/***************************************************************************************************************************
**��������:	 	cmd_user
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
static void cmd_user(const char *arg,struct ftpd_msgstate *fsm)
{
	LIBMCU_DEBUG(FTP_DEBUG,("\r\n cmd_user "));
	send_msg(msg331);
	msgcount = 0;
	fsm->state = FTPD_PASS;
}
/***************************************************************************************************************************
**��������:	 	cmd_pass
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
static void cmd_pass(const char *arg,struct ftpd_msgstate *fsm)
{
	LIBMCU_DEBUG(FTP_DEBUG,("\r\n cmd_pass "));
	send_msg(msg230);
	fsm->state = FTPD_IDLE;
}
/***************************************************************************************************************************
**��������:	 	cmd_port
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
static void cmd_port(const char *arg,struct ftpd_msgstate *fsm)
{
	BYTE i = 0;
	BYTE data[6];
	char chdata[5];
	BYTE cnt = 0;
	BYTE cnt1 = 0;
	char *ch = (char *)arg;
	LIBMCU_DEBUG(FTP_DEBUG,("\r\n cmd_port "));
	memset(chdata,'\0',sizeof(chdata));
	memset(data,0,sizeof(data));
	for(i = 0;i < fsm->len;i++)
	{
		if((' ' == ch[i]) || ('.' == ch[i]) || (',' == ch[i])|| ('\r' == ch[i])|| ('\n' == ch[i]))
		{
			if(cnt)
			{
				cnt = 0;
				data[cnt1] = atol(chdata);
				printf("\r\n data %d ",data[cnt1]);
				cnt1 = (cnt1 + 1) % sizeof(data);
				memset(chdata,'\0',sizeof(chdata));		
			}
		}
		else if((ch[i] >= '0') && (ch[i] <= '9'))
		{
			chdata[cnt] = ch[i];
			cnt = (cnt + 1) % sizeof(chdata);
		}
	}
	
	IP4_ADDR(&fsm->dataip, (BYTE) data[0], (BYTE) data[1], (BYTE) data[2], (BYTE) data[3]);
	fsm->dataport = ((UINT16) (data[4] << 8) | (UINT16) data[5]);
	send_msg(msg200);
	
	printf("\r\n dataip %u dataport %d ",fsm->dataip.addr,fsm->dataport);
}
/***************************************************************************************************************************
**��������:	 	cmd_quit
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
static void cmd_quit(const char *arg,struct ftpd_msgstate *fsm)
{

}
/***************************************************************************************************************************
**��������:	 	cmd_cwd
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
static void cmd_cwd(const char *arg,struct ftpd_msgstate *fsm)
{
	LIBMCU_DEBUG(FTP_DEBUG,("\r\n cmd_cwd "));
	if(pDirPatch)
	{
		send_msg(msg250);
	}
	else
	{
		send_msg(msg550);	
	}
}
/***************************************************************************************************************************
**��������:	 	cmd_cdup
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
static void cmd_cdup(const char *arg,struct ftpd_msgstate *fsm)
{

}
/***************************************************************************************************************************
**��������:	 	cmd_pwd
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
static void cmd_pwd(const char *arg,struct ftpd_msgstate *fsm)
{
	LIBMCU_DEBUG(FTP_DEBUG,("\r\n cmd_pwd "));
	send_msg(msg257PWD,"d:\\root");//pDirPatch
}

/***************************************************************************************************************************
**��������:	 	cmd_nlst
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
static void cmd_nlst(const char *arg,struct ftpd_msgstate *fsm)
{
	LIBMCU_DEBUG(FTP_DEBUG,("\r\n cmd_nlst "));
	cmd_list_common(pDirPatch,fsm);
	send_msg(msg125);
	send_msg(msg150);
}
/***************************************************************************************************************************
**��������:	 	cmd_list
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
static void cmd_list(const char *arg,struct ftpd_msgstate *fsm)
{
	LIBMCU_DEBUG(FTP_DEBUG,("\r\n cmd_list "));
	cmd_list_common(pDirPatch,fsm);
	send_msg(msg150);
	send_msg(msg125);
}
/***************************************************************************************************************************
**��������:	 	cmd_retr
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
static void cmd_retr(const char *arg,struct ftpd_msgstate *fsm)
{

}
/***************************************************************************************************************************
**��������:	 	cmd_stor
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
static void cmd_stor(const char *arg,struct ftpd_msgstate *fsm)
{

}
/***************************************************************************************************************************
**��������:	 	cmd_stor
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
static void cmd_noop(const char *arg,struct ftpd_msgstate *fsm)
{

}
/***************************************************************************************************************************
**��������:	 	cmd_syst
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
static void cmd_syst(const char *arg,struct ftpd_msgstate *fsm)
{
	LIBMCU_DEBUG(FTP_DEBUG,("\r\n cmd_syst "));
	send_msg(msg214SYST, "UNIX");
}
/***************************************************************************************************************************
**��������:	 	cmd_abrt
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
static void cmd_abrt(const char *arg,struct ftpd_msgstate *fsm)
{

}
/***************************************************************************************************************************
**��������:	 	cmd_type
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
static void cmd_type(const char *arg,struct ftpd_msgstate *fsm)
{
	LIBMCU_DEBUG(FTP_DEBUG,("\r\n cmd_type "));
	send_msg(msg502);
}
/***************************************************************************************************************************
**��������:	 	cmd_mode
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
static void cmd_mode(const char *arg,struct ftpd_msgstate *fsm)
{

}
/***************************************************************************************************************************
**��������:	 	cmd_rnfr
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
static void cmd_rnfr(const char *arg,struct ftpd_msgstate *fsm)
{

}
/***************************************************************************************************************************
**��������:	 	cmd_rnto
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
static void cmd_rnto(const char *arg,struct ftpd_msgstate *fsm)
{

}
/***************************************************************************************************************************
**��������:	 	cmd_mkd
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
static void cmd_mkd(const char *arg,struct ftpd_msgstate *fsm)
{

}
/***************************************************************************************************************************
**��������:	 	cmd_mkd
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
static void cmd_rmd(const char *arg,struct ftpd_msgstate *fsm)
{

}
/***************************************************************************************************************************
**��������:	 	cmd_mkd
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
static void cmd_dele(const char *arg,struct ftpd_msgstate *fsm)
{
	LIBMCU_DEBUG(FTP_DEBUG,("\r\n cmd_dele "));
}
/***************************************************************************************************************************
**��������:	 	cmd_pasv
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
static void cmd_pasv(const char *arg,struct ftpd_msgstate *fsm)
{
	LIBMCU_DEBUG(FTP_DEBUG,("\r\n cmd_pasv "));
	send_msg(msg227);
}	

static struct ftpd_command ftpd_commands[] = 
{
	{N0,"USER",cmd_user},
	{N1,"PASS",cmd_pass},
	{N2,"PORT",cmd_port},
	{N3,"QUIT",cmd_quit},
	{N4,"CWD", cmd_cwd},
	{N5,"CDUP",cmd_cdup},
	{N6,"PWD", cmd_pwd},
	{N7,"XPWD", cmd_pwd},
	{N8,"NLST", cmd_nlst},
	{N9,"LIST", cmd_list},
	{N10,"RETR", cmd_retr},
	{N11,"STOR", cmd_stor},
	{N12,"NOOP", cmd_noop},
	{N13,"SYST", cmd_syst},
	{N14,"ABOR", cmd_abrt},
	{N15,"TYPE", cmd_type},
	{N16,"MODE", cmd_mode},
	{N17,"RNFR", cmd_rnfr},
	{N18,"RNTO", cmd_rnto},
	{N19,"MKD", cmd_mkd},
	{N20,"XMKD", cmd_mkd},
	{N21,"RMD", cmd_rmd},
	{N22,"XRMD", cmd_rmd},
	{N23,"DELE", cmd_dele},
//	{N24,"PASV",cmd_pasv},
	{MAX,NULL, NULL}
};
/***************************************************************************************************************************
**��������:	 	msg_weclome
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
void msg_weclome(struct netconn *ftpconn)
{
	send_msg(msg220);
}
/***************************************************************************************************************************
**��������:	 	ftpd_msgrecv
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
static err_t ftpd_msgrecv(char *p,UINT16 len)
{
	BYTE i;
	struct ftpd_command *ftp_cmd = &ftpd_commands[0];
	for(i = 0;i < MAX;i++)
	{	
		printf("\r\n ftpd_msgrecv i = %d len = %d ",i,strlen(ftp_cmd->cmd));
		ftp_cmd = &ftpd_commands[i];
		if(!memcmp(p,ftp_cmd->cmd,strlen(ftp_cmd->cmd)))
		{
			printf("\r\n ftp_cmd->func");
			if(ftp_cmd->func)
			{
				fsm->len = len;
				ftp_cmd->func(p,fsm);
			}
			break;
		}	
	}
	
	if(MAX == i)
	{
		send_msg(msg500);
	}
	return ERR_OK;
}
/***************************************************************************************************************************
**��������:	 	ftp_poll_fn
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
err_t ftp_poll_fn(void *arg, struct tcp_pcb *tpcb)
{
	LIBMCU_DEBUG(ETHERNTE_DEBUG,("\r\n ftp_poll_fn"));
	if(fsm->datafs->bConnect)
	{
		switch (fsm->state) 
		{
			case FTPD_LIST:
				send_next_directory(fsm->datafs,fsm->pftpinfo->ftpRAWDataMsg, 0);
				break;
			case FTPD_NLST:
				send_next_directory(fsm->datafs,fsm->pftpinfo->ftpRAWDataMsg, 1);
				break;
			case FTPD_RETR:
				//send_file(fsm->datafs, fsm->datapcb);
				break;
			default:
				break;
		}
	}
	return ERR_OK;
}
/***************************************************************************************************************************
**��������:	 	ftp_connected_fn
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
err_t ftp_connected_fn(void *arg, struct tcp_pcb *tpcb, err_t err)
{
	LIBMCU_DEBUG(ETHERNTE_DEBUG,("\r\n ftp_connected_fn"));
	fsm->datafs->bConnect = 1; 
	return ERR_OK;
}
/***************************************************************************************************************************
**��������:	 	ftpd_init
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
void ftpd_init(FTP_INFO *pftpInfo,FTP_FUNC *pftpFunc)
{
	fsm = &msgstate;
	fsm->state = FTPD_USER;
	memset(fsm->curPatch,'\0',sizeof(fsm->curPatch));
	datastate.bConnect = 0;
	pDirPatch = fsm->curPatch;
	fsm->patchLen = strlen("root");
	memcpy(pDirPatch,"root",fsm->patchLen);
	fsm->pftpfunc = pftpFunc;
	fsm->pftpinfo = pftpInfo;

}
/***************************************************************************************************************************
**��������:	 	ftp_in
**��������:	 	
**��ڲ���:
**���ز���:
***************************************************************************************************************************/
void ftp_in(char *p,UINT16 len)
{
	ftpd_msgrecv(p,len);
}
