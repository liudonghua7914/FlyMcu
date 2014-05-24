#ifndef 	__FSDEV_H__
#define		__FSDEV_H__
#include "fs_port.h"


typedef struct 
{
	FS_FARCHARPTR name;	
	int (*dev_status)(FS_u32 id);	
	int (*dev_read)(FS_u32 id, FS_u32 block, void *buffer);	
	int (*dev_write)(FS_u32 id, FS_u32 block, void *buffer);	
	int (*dev_ioctl)(FS_u32 id, FS_i32 cmd, FS_i32 aux, void *buffer);

}FS__device_type, FS__DEVICE_TYPE;


#endif


