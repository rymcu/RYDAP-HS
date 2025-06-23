/**
 * @file    SWD_flash.c
 * @brief   ???SWDЭ???MCU??FLASH???
 */

#include "dap_flash.h"
#include "swd_host.h"
//每添加一个类型MCU，增加一个变量
extern  program_target_t flash_algo_F10x;
extern  program_target_t flash_algo_F429;
program_target_t* pflash_algo;

void dap_offline_algo_select(MCU_Type_t McuType)
{
    switch (McuType) {
        case F103x:
            pflash_algo = &flash_algo_F10x;
            break;
        case F429:
            pflash_algo = &flash_algo_F429;
            break;
        default:
            pflash_algo = &flash_algo_F10x;
    }


}

error_t target_flash_init(uint32_t flash_start)
{
    if(0 == swd_set_target_state_hw(RESET_PROGRAM))
        {
            return ERROR_RESET;
        }

    // ?????????????MCU??SRAM?????????
    if(0 == swd_write_memory(pflash_algo->algo_start, (uint8_t *)pflash_algo->algo_blob, pflash_algo->algo_size))
        {
            return ERROR_ALGO_DL;
        }

    if(0 == swd_flash_syscall_exec(&pflash_algo->sys_call_s, pflash_algo->init, flash_start, 0, 0, 0,0))
        {
            return ERROR_INIT;
        }

    return ERROR_SUCCESS;
}

error_t target_flash_uninit(void)
{
    swd_set_target_state_hw(RESET_RUN);

    swd_off();
    return ERROR_SUCCESS;
}

error_t target_flash_program_page(uint32_t addr, const uint8_t *buf, uint32_t size)
{
    while(size > 0)
        {
            uint32_t write_size = size > pflash_algo->program_buffer_size ? pflash_algo->program_buffer_size : size;

            // Write page to buffer
            if(!swd_write_memory(pflash_algo->program_buffer, (uint8_t *)buf, write_size))
                {
                    return ERROR_ALGO_DATA_SEQ;
                }

            // Run flash programming
            if(!swd_flash_syscall_exec(&pflash_algo->sys_call_s,
                                       pflash_algo->program_page,
                                       addr,
                                       pflash_algo->program_buffer_size,
                                       pflash_algo->program_buffer,
                                       0,0))
                {
                    return ERROR_WRITE;
                }

            addr += write_size;
            buf  += write_size;
            size -= write_size;
        }

    return ERROR_SUCCESS;
}

error_t target_flash_erase_sector(uint32_t addr)
{
    if(0 == swd_flash_syscall_exec(&pflash_algo->sys_call_s, pflash_algo->erase_sector, addr, 0, 0, 0,0))
        {
            return ERROR_ERASE_SECTOR;
        }

    return ERROR_SUCCESS;
}

error_t target_flash_erase_chip(void)
{
    error_t status = ERROR_SUCCESS;

    if(0 == swd_flash_syscall_exec(&pflash_algo->sys_call_s, pflash_algo->erase_chip, 0, 0, 0, 0,0))
        {
            return ERROR_ERASE_ALL;
        }

    return status;
}
