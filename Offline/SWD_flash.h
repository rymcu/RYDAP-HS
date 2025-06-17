#ifndef __SWD_FLASH_H__
#define __SWD_FLASH_H__

#include <stdint.h>
#include "error.h"

#include "swd_host.h"

//每添加一个类型MCU，增加一个类型
typedef enum {
    F103x = 0,				/* (0)*/
    F429
} MCU_Type_t;

error_t target_flash_init(uint32_t flash_start);
error_t target_flash_uninit(void);
error_t target_flash_program_page(uint32_t addr, const uint8_t *buf, uint32_t size);
error_t target_flash_erase_sector(uint32_t addr);
error_t target_flash_erase_chip(void);
extern void Findflash(MCU_Type_t McuType);

#endif // __SWD_FLASH_H__
