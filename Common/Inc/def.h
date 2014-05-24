#ifndef _DEF_H_
#define _DEF_H_

#include "Lpc_types.h"

typedef Bool BOOL;

typedef unsigned char  BYTE;
typedef unsigned int   UINT;
typedef unsigned short WORD;
typedef unsigned short UINT16;
typedef unsigned long  UINT32;
typedef unsigned long  DWORD;
typedef unsigned long  ULONG;

typedef unsigned char  U8;
typedef unsigned char  u8;
typedef unsigned short U16;
typedef unsigned short u16;
typedef unsigned int  U32;
typedef unsigned int  u32;

typedef unsigned char  Uint8;
typedef unsigned short Uint16;
typedef unsigned long  Uint32;

typedef unsigned char  uint8;
typedef unsigned short uint16;
typedef unsigned long  uint32;

typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;

extern void _DI(void);
extern void _EI(void);


#endif

