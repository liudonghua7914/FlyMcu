/*
*********************************************************************************************************
*                                             uC/FS V4
*                                     The Embedded File System
*
*                         (c) Copyright 2008-2013; Micrium, Inc.; Weston, FL
*
*               All rights reserved. Protected by international copyright laws.
*
*               uC/FS is provided in source form to registered licensees ONLY.  It is
*               illegal to distribute this source code to any third party unless you receive
*               written permission by an authorized Micrium representative.  Knowledge of
*               the source code may NOT be used to develop a similar product.
*
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*
*               You can contact us at www.micrium.com.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                      FILE SYSTEM DEVICE DRIVER
*
*                                             SD/MMC CARD
*                                               SPI MODE
*
* Filename      : fs_dev_sd_spi.h
* Version       : v4.05.03.00
* Programmer(s) : BAN
*********************************************************************************************************
* Reference(s)  : (1) SD Card Association.  "Physical Layer Simplified Specification Version 2.00".
*                     July 26, 2006.
*
*                 (2) JEDEC Solid State Technology Association.  "MultiMediaCard (MMC) Electrical
*                     Standard, High Capacity".  JESD84-B42.  July 2007.
*********************************************************************************************************
* Note(s)       : (1) This driver has been tested with MOST SD/MMC media types, including :
*
*                     (a) Standard capacity SD cards, v1.x & v2.0.
*                     (b) SDmicro cards.
*                     (c) High capacity SD cards (SDHC)
*                     (d) MMC
*                     (e) MMCplus
*
*                     It should also work with devices conformant to the relevant SD or MMC specifications.
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                               MODULE
*********************************************************************************************************
*/

#ifndef  FS_DEV_SD_SPI_PRESENT
#define  FS_DEV_SD_SPI_PRESENT


/*
*********************************************************************************************************
*                                               EXTERNS
*********************************************************************************************************
*/

#ifdef   FS_DEV_SD_SPI_MODULE
#define  FS_DEV_SD_SPI_EXT
#else
#define  FS_DEV_SD_SPI_EXT  extern
#endif


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include  <fs_dev.h>
#include  <fs_dev_sd.h>


#define	FS_DEV_SD_SPI_CFG_CRC_EN
/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                             DATA TYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                          GLOBAL VARIABLES
*********************************************************************************************************
*/

extern             const  FS_DEV_API  FSDev_SD_SPI;
FS_DEV_SD_SPI_EXT         FS_CTR      FSDev_SD_SPI_UnitCtr;


/*
*********************************************************************************************************
*                                               MACRO'S
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void         FSDev_SD_SPI_QuerySD           (CPU_CHAR        *name_dev,     /* Get info about SD/MMC card.              */
                                             FS_DEV_SD_INFO  *p_info,
                                             FS_ERR          *p_err);

void         FSDev_SD_SPI_RdCID             (CPU_CHAR        *name_dev,     /* Read SD/MMC Card ID register.            */
                                             CPU_INT08U      *p_dest,
                                             FS_ERR          *p_err);

void         FSDev_SD_SPI_RdCSD             (CPU_CHAR        *name_dev,     /* Read SD/MMC Card-Specific Data register. */
                                             CPU_INT08U      *p_dest,
                                             FS_ERR          *p_err);


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*                               DEFINED IN BSP'S 'fs_dev_sd_spi_bsp.c'
*
* Note(s) : (1) SPI functions MUST be gathered into a SPI API structure.
*********************************************************************************************************
*/

extern  const  FS_DEV_SPI_API  FSDev_SD_SPI_BSP_SPI;


/*
*********************************************************************************************************
*                                        CONFIGURATION ERRORS
*********************************************************************************************************
*/

#ifndef  FS_DEV_SD_SPI_CFG_CRC_EN
#error  "FS_DEV_SD_SPI_CFG_CRC_EN                not #define'd in 'fs_cfg.h'"
#error  "                                  [MUST be  DEF_DISABLED]          "
#error  "                                  [     ||  DEF_ENABLED ]          "

#elif  ((FS_DEV_SD_SPI_CFG_CRC_EN  != DEF_DISABLED) && \
        (FS_DEV_SD_SPI_CFG_CRC_EN  != DEF_ENABLED ))
#error  "FS_DEV_SD_SPI_CFG_CRC_EN          illegally #define'd in 'fs_cfg.h'"
#error  "                                  [MUST be  DEF_DISABLED]          "
#error  "                                  [     ||  DEF_ENABLED ]          "

#elif   (FS_DEV_SD_SPI_CFG_CRC_EN  == DEF_ENABLED)

#if     (EDC_CRC_CFG_CRC16_1021_EN == DEF_DISABLED)
#error  "EDC_CRC_CFG_CRC16_1021_EN         illegally #define'd in 'crc_cfg.h'"
#error  "                                  [MUST be  DEF_ENABLED]            "
#endif

#endif


/*
*********************************************************************************************************
*                                             MODULE END
*********************************************************************************************************
*/

#endif                                                          /* End of SD SPI module include.                        */
