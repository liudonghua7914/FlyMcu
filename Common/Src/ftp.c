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
	标准FTP信息:	
	
	
	

  序列		响应代码						解释说明
	1.		  110						新文件指示器上的重启标记
	2.		  120						服务器贮备就绪时间(分钟数)
	3.		  125						打开数据连接,开始传输
	4.		  150						打开连接
	5.		  200						成功
	6.		  202						命令没有执行
	7.		  211						系统状态回复
	8.		  212						目录状态回复
	9.		  213						文件状态回复
	10.		  214						帮助信息回复
	11.		  215						系统类型回复
	12.		  220					    服务就绪
	13.		  221						退出网络
	14.		  225						打开数据连接
	15. 	  227						结束数据连接
	16.       230						登陆英特网
	17.		  250						文件行为完成
	18.		  257						文件路径名建立
	19.		  331						要求密码
	20.		  332						要求账号
	21. 	  350						文件行为暂停
	22.		  421						服务关闭
	23.		  425						无法打开数据连接
	24.		  426						结束连接
	25.		  450						文件不可以
	26.		  451						遇到本地错误
	27.		  452						磁盘空间不足
	28.		  500						无效命令
	29.		  501						错误参数
	30.		  502						命令没有执行
	31.		  503						错误指令序列
	32.		  504						无效命令参数
	33.		  530						未登录网络
	34.		  532						存储文件需要账号
	35.		  550						文件不可以
	36.		  551						不知道页的类型
	37.		  552						超出存储分配
	38.		  553						文件名不允许
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





static struct ftpd_msgstate msgstate;
static struct ftpd_msgstate *fsm;
static char buffer[256];

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
**函数名称:	 	getFTPMsgState
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
BYTE getFTPMsgState(void)
{
	return fsm->state;
}
/***************************************************************************************************************************
**函数名称:	 	setFTPMsgState
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
void setFTPMsgState(BYTE state)
{
	fsm->state = state;
}
/***************************************************************************************************************************
**函数名称:	 	sfifo_init
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
static int sfifo_init(sfifo_t *f,int size)
{
	return 0;
}
/***************************************************************************************************************************
**函数名称:	 	sfifo_close
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
static void sfifo_close(sfifo_t *f)
{

}
/***************************************************************************************************************************
**函数名称:	 	sfifo_flush
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
static void sfifo_flush(sfifo_t *f)
{
	
}
/***************************************************************************************************************************
**函数名称:	 	sfifo_write
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
static int sfifo_write(sfifo_t *f)
{
	return 0;
}
/***************************************************************************************************************************
**函数名称:	 	sfifo_read
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
static int sfifo_read(sfifo_t *f,void *buf,int len)
{
	return 0;
}

/***************************************************************************************************************************
**函数名称:	 	send_msg
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
static void send_msg(char *msg, ...)
{
	va_list arg;
	va_start(arg, msg);
	memset(buffer,'\0',sizeof(buffer));
	vsprintf(buffer, msg, arg);
	va_end(arg);
	strcat(buffer, "\r\n");
	fsm->ftp_write_data(buffer,strlen(buffer));
}
/***************************************************************************************************************************
**函数名称:	 	cmd_user
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
static void cmd_user(const char *arg,struct ftpd_msgstate *fsm)
{
	LIBMCU_DEBUG(FTP_DEBUG,("\r\n cmd_user "));
	send_msg(msg331);
	fsm->state = FTPD_PASS;
}
/***************************************************************************************************************************
**函数名称:	 	cmd_pass
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
static void cmd_pass(const char *arg,struct ftpd_msgstate *fsm)
{
	LIBMCU_DEBUG(FTP_DEBUG,("\r\n cmd_pass "));
	send_msg(msg230);
	fsm->state = FTPD_IDLE;
}
/***************************************************************************************************************************
**函数名称:	 	cmd_port
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
static void cmd_port(const char *arg,struct ftpd_msgstate *fsm)
{
	
	BYTE i = 0;
	BYTE offset = 1;
	BYTE data[6];
	BYTE cnt = 0;
	char *ch = (char *)arg;
	char *pt = &ch[strlen("PORT")];
	LIBMCU_DEBUG(FTP_DEBUG,("\r\n cmd_syst "));
	for(i = 0;i < fsm->len;i+= offset)
	{
		if((pt[i] != ' ') && (pt[i] != '.') && (pt[i] != ','))
		{
			//offset = snprintf(&data[cnt],sizeof(data[cnt],"%s",&pt[i]);
			printf("\r\n offset = %d ",offset);
			cnt = (cnt + 1) % (sizeof(data));
		}
		else
		{
			offset = 1;
		}
	}
	
}
/***************************************************************************************************************************
**函数名称:	 	cmd_quit
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
static void cmd_quit(const char *arg,struct ftpd_msgstate *fsm)
{

}
/***************************************************************************************************************************
**函数名称:	 	cmd_cwd
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
static void cmd_cwd(const char *arg,struct ftpd_msgstate *fsm)
{

}
/***************************************************************************************************************************
**函数名称:	 	cmd_cdup
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
static void cmd_cdup(const char *arg,struct ftpd_msgstate *fsm)
{

}
/***************************************************************************************************************************
**函数名称:	 	cmd_pwd
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
static void cmd_pwd(const char *arg,struct ftpd_msgstate *fsm)
{

}
/***************************************************************************************************************************
**函数名称:	 	cmd_nlst
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
static void cmd_nlst(const char *arg,struct ftpd_msgstate *fsm)
{

}
/***************************************************************************************************************************
**函数名称:	 	cmd_list
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
static void cmd_list(const char *arg,struct ftpd_msgstate *fsm)
{

}
/***************************************************************************************************************************
**函数名称:	 	cmd_retr
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
static void cmd_retr(const char *arg,struct ftpd_msgstate *fsm)
{

}
/***************************************************************************************************************************
**函数名称:	 	cmd_stor
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
static void cmd_stor(const char *arg,struct ftpd_msgstate *fsm)
{

}
/***************************************************************************************************************************
**函数名称:	 	cmd_stor
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
static void cmd_noop(const char *arg,struct ftpd_msgstate *fsm)
{

}
/***************************************************************************************************************************
**函数名称:	 	cmd_syst
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
static void cmd_syst(const char *arg,struct ftpd_msgstate *fsm)
{
	LIBMCU_DEBUG(FTP_DEBUG,("\r\n cmd_syst "));
	send_msg(msg214SYST, "UNIX");
}
/***************************************************************************************************************************
**函数名称:	 	cmd_abrt
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
static void cmd_abrt(const char *arg,struct ftpd_msgstate *fsm)
{

}
/***************************************************************************************************************************
**函数名称:	 	cmd_type
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
static void cmd_type(const char *arg,struct ftpd_msgstate *fsm)
{

}
/***************************************************************************************************************************
**函数名称:	 	cmd_mode
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
static void cmd_mode(const char *arg,struct ftpd_msgstate *fsm)
{

}
/***************************************************************************************************************************
**函数名称:	 	cmd_rnfr
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
static void cmd_rnfr(const char *arg,struct ftpd_msgstate *fsm)
{

}
/***************************************************************************************************************************
**函数名称:	 	cmd_rnto
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
static void cmd_rnto(const char *arg,struct ftpd_msgstate *fsm)
{

}
/***************************************************************************************************************************
**函数名称:	 	cmd_mkd
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
static void cmd_mkd(const char *arg,struct ftpd_msgstate *fsm)
{

}
/***************************************************************************************************************************
**函数名称:	 	cmd_mkd
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
static void cmd_rmd(const char *arg,struct ftpd_msgstate *fsm)
{

}
/***************************************************************************************************************************
**函数名称:	 	cmd_mkd
**函数功能:	 	
**入口参数:
**返回参数:
***************************************************************************************************************************/
static void cmd_dele(const char *arg,struct ftpd_msgstate *fsm)
{
	LIBMCU_DEBUG(FTP_DEBUG,("\r\n cmd_dele "));
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
	{MAX,NULL, NULL}
};

/***************************************************************************************************************************
**函数名称:	 	ftpd_msgrecv
**函数功能:	 	
**入口参数:
**返回参数:
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
	return ERR_OK;
}

void ftpd_init(void)
{
	fsm = &msgstate;
	fsm->state = FTPD_USER;
	fsm->ftp_write_data = fptWriteData;
}

