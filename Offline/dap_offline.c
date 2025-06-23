#include "dap_offline.h"

#define PageSize 1024  //根据不同的单片机内部flash设置大小
#define BeginAddr 0x08000000 //程序下载起始地址，由可执行文件决定

uint8_t dap_offline_download (FATFS fs, FIL fnew, char *path, uint32_t McuType) {
    uint8_t rData[PageSize];
    uint32_t readflag = 1;
    uint32_t addr = 0;
    UINT fnum;

    dap_offline_algo_select (McuType);  // 选择对对应的MCU型号
    // step 1
    if(!swd_init_debug()) return 1;
    // step 2
    if(target_flash_init (BeginAddr)) return 2;
    // step 3
    if(target_flash_erase_chip()) return 3;//此处可改为部分擦除
    // setp 4
    if(f_open (&fnew, path, FA_OPEN_EXISTING | FA_READ))return 4;
    printf("download bin info:%s(%0.2fKB)\r\n", path,(float)f_size(&fnew)/1024.0);
    //step 5
    while (readflag) 
    {
        if(f_read (&fnew, rData, PageSize, &fnum)) return 5;
        if (fnum < PageSize) readflag = 0;

        if(target_flash_program_page (BeginAddr + addr, (uint8_t *)&rData[0], PageSize))return 55;
        addr += PageSize;
    }
    f_close (&fnew);  // 读取完毕，关闭文件。
    // step 6
    if(!swd_init_debug()) return 6;
    // step 7
    swd_set_target_reset(0);  // 复位运行
    printf ("dap offline download ok\r\n");
    return 0;
}

void EraseSectorTest (void) {
    uint32_t error;
    uint32_t addr = 0x08020000;

    dap_offline_algo_select (1);
    swd_init_debug();
    target_flash_init (BeginAddr);
    error = target_flash_erase_sector (addr);
    // 经测试：
    // erase_sector函数单次擦除1页，其中addr为实际地址，或者实际地址减去开始地址
    // sector和page的关系
    // 1*sector = N*page
    // 怎么获得：sector size？
    // 根据具体的型号计算sector大小及分布
    // 获得要下载程序的大小以及开始地址，计算占用哪些sector,再擦除！
    // algo中program_buffer_size = PageSize
    //
    printf("erase sector:0x%X,error = %d!\r\n",addr,error);
}
