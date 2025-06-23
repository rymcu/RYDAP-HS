/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"     /* Obtains integer types */
#include "diskio.h" /* Declarations of disk functions */
#include "bsp_spi_flash.h" //hugh
/* Definitions of physical drive number for each drive */
#define DEV_RAM 0 /* Example: Map Ramdisk to physical drive 0 */
#define DEV_MMC 1 /* Example: Map MMC/SD card to physical drive 1 */
#define DEV_USB 2 /* Example: Map USB MSD to physical drive 2 */

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
    BYTE pdrv /* Physical drive nmuber to identify the drive */
) {
    //printf ("disk_status\r\n");
    Stat = STA_NOINIT;
    if (pdrv == 0)
        Stat = RES_OK;
    return Stat;
}

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
    BYTE pdrv /* Physical drive nmuber to identify the drive */
) {
    //printf ("disk_initialize\r\n");
    Stat = STA_NOINIT;
    switch (pdrv) {
    case 1:  // SD
        Stat = RES_PARERR;
        break;
    case 0:
        Flash_Init();printf ("SPI flash W25Qx init done!\r\n");
        Stat = RES_OK;
        break;
    default:
        Stat = RES_PARERR;
    }
    return Stat;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
    BYTE pdrv,    /* Physical drive nmuber to identify the drive */
    BYTE *buff,   /* Data buffer to store read data */
    LBA_t sector, /* Start sector in LBA */
    UINT count    /* Number of sectors to read */
) {
    //printf ("disk_read\r\n");
    if (pdrv == 0) {
        Flash_ReadData (sector * FLASH_SECTOR_SIZE, (uint8_t *)buff, count * FLASH_SECTOR_SIZE);
        return RES_OK;
    } else {
        return RES_PARERR;
    }
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
    BYTE pdrv,        /* Physical drive nmuber to identify the drive */
    const BYTE *buff, /* Data to be written */
    LBA_t sector,     /* Start sector in LBA */
    UINT count        /* Number of sectors to write */
) {
    //printf ("disk_write\r\n");
    if (pdrv == 0) {
        Flash_SectorErase (sector * FLASH_SECTOR_SIZE);
        FLASH_WriteData (sector * FLASH_SECTOR_SIZE, (uint8_t *)buff, count * FLASH_SECTOR_SIZE);
        return RES_OK;
    } else {
        return RES_PARERR;
    }
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
    BYTE pdrv, /* Physical drive nmuber (0..) */
    BYTE cmd,  /* Control code */
    void *buff /* Buffer to send/receive control data */
) {
    //printf ("disk_ioctl\r\n");
    DRESULT res = RES_ERROR;

    if (pdrv == 0) {
        switch (cmd) {
        case CTRL_SYNC:
            res = RES_OK;
            break;
        case GET_SECTOR_SIZE:
            *(DWORD *)buff = FLASH_SECTOR_SIZE;
            res = RES_OK;
            break;
        case GET_SECTOR_COUNT:
            *(DWORD *)buff = FLASH_SECTOR_COUNT;
            res = RES_OK;
            break;
        case GET_BLOCK_SIZE:
            *(DWORD *)buff = 1;
            res = RES_OK;
            break;
        default:
            res = RES_PARERR;
        }
    } else {
        res = RES_PARERR;
    }
    return res;
}

// User defined function to give a current time to fatfs module
DWORD get_fattime (void) {
    return 0;
}
