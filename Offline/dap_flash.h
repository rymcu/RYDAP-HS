#ifndef __SWD_FLASH_H__
#define __SWD_FLASH_H__

#include <stdint.h>
#include "error.h"

typedef struct {
    uint32_t breakpoint;
    uint32_t static_base;
    uint32_t stack_pointer;
} program_syscall_t;

typedef struct {
    const uint32_t  init;
    const uint32_t  uninit;
    const uint32_t  erase_chip;
    const uint32_t  erase_sector;
    const uint32_t  program_page;
    const program_syscall_t sys_call_s;
    const uint32_t  program_buffer;
    const uint32_t  algo_start;
    const uint32_t  algo_size;
    const uint32_t *algo_blob;
    const uint32_t  program_buffer_size;
} program_target_t;

typedef struct {
    const uint32_t start;
    const uint32_t size;
} sector_info_t;

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
extern void dap_offline_algo_select(MCU_Type_t McuType);

extern uint32_t flash_code1[83];
extern uint32_t flash_code[100];
extern program_target_t flash_algo_F429;
extern program_target_t flash_algo_F10x;
#endif // __SWD_FLASH_H__
