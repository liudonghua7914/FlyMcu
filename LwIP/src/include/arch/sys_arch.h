#ifndef 	__SYSARCH_H__
#define		__SYSARCH_H__


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "Err.h"
#include "includes.h"

#define		SYS_MBOX_NULL 	NULL
#define		SYS_SEM_NULL 	NULL

#define		LWIP_THREAD_TASK_PRIO_MIN	5	
#define 	LWIP_THREAD_TASK_PRIO_MAX	7



#define		LWIP_THREAD_TASK_MAX 		4
#define		LWIP_THREAD_TASK_STACK		200

#define		MAX_Q						3
#define 	MAX_QUEUE_ENTRIES			30

#define sys_arch_mbox_tryfetch(mbox,msg)  sys_arch_mbox_fetch(mbox,msg,1)

typedef OS_EVENT* sys_sem_t;
typedef OS_EVENT* sys_mutex_t;
typedef OS_EVENT* sys_mbox_t;
typedef unsigned char sys_prot_t;
typedef unsigned char sys_thread_t;

#if 0
void sys_init(void);
err_t sys_sem_new(sys_sem_t *sem, u8_t count);
void sys_sem_free(sys_sem_t *sem);
void sys_sem_signal(sys_sem_t *sem);
u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout);
int sys_sem_valid(sys_sem_t *sem);
void sys_sem_set_invalid(sys_sem_t *sem);
err_t sys_mbox_new(sys_mbox_t *mbox, int size);
void sys_mbox_free(sys_mbox_t *mbox);
void sys_mbox_post(sys_mbox_t *mbox, void *msg);
err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg);
u32_t sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout);
u32_t sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg);
int sys_mbox_valid(sys_mbox_t *mbox);
void sys_mbox_set_invalid(sys_mbox_t *mbox);
#endif

#endif
