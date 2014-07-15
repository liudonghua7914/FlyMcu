#ifndef 	__CC_H__
#define		__CC_H__


/* 
	该文件参考lwip/doc/sys_arch.txt 文件 
*/

/*
	Architecture environment, some compiler specific, some
	environment specific (probably should move env stuff 
    to sys_arch.h.)
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>


#define BYTE_ORDER  LITTLE_ENDIAN

/*
	Typedefs for the types used by lwip
*/
typedef unsigned char u8_t;
typedef signed char s8_t;
typedef unsigned  short u16_t;
typedef signed short s16_t;
typedef unsigned int u32_t;
typedef signed int s32_t;
typedef uintptr_t mem_ptr_t;

typedef unsigned int sys_port_t;

/*
	 Compiler hints for packing lwip's structures
*/
#define PACK_STRUCT_FIELD(x)    x
#define PACK_STRUCT_STRUCT  __attribute__((packed))
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_END

/*
	Define (sn)printf formatters for these lwIP types 
*/
#define U16_F "hu"
#define S16_F "hd"
#define X16_F "hx"
#define U32_F "u"
#define S32_F "d"
#define X32_F "x"

/*
	Platform specific diagnostic output
*/
#define LWIP_PLATFORM_DIAG(x)   	printf x

   								 
#define LWIP_PLATFORM_ASSERT(x) 	do{                \
									}while (0)


											



#endif
