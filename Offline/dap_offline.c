#include "dap_offline.h"

#define PageSize 1024  //根据不同的单片机内部flash设置大小
#define BeginAddr 0x08000000 //程序下载起始地址，由可执行文件决定

uint8_t dap_offline_download (FATFS fs, FIL fnew, char *path, uint32_t McuType) {
    uint8_t error, rData[PageSize];
    uint32_t readflag = 1;
    uint32_t addr = 0;
    UINT fnum;

    printf ("dap_offline_download 1\r\n");
    dap_offline_algo_select (McuType);  // 选择对对应的MCU型号
    for (int i = 0; i < PageSize; ++i) { rData[i] = 0x55; }
    // step 1
    printf ("dap_offline_download step 1\r\n");
    error = swd_init_debug();
    printf ("dap_offline_download step 1.5\r\n");
    if (!error)
        return 5;
    // step 2
    printf ("dap_offline_download step 2\r\n");
    error = target_flash_init (BeginAddr);
    if (error)
        return 6;
    // step 3
    printf ("dap_offline_download step 3\r\n");
    error = target_flash_erase_chip();
    if (error)
        return 7;
    // setp 4
    printf ("dap_offline_download step 4\r\n");
    Delay_Ms (100);
    error = f_open (&fnew, path, FA_OPEN_EXISTING | FA_READ);
    if (error)
        return 77;
     printf("download:%s(%0.2fKB)\r\n", path,(float)f_size(&fnew)/1024.0);
    // HAL_GPIO_WritePin(GPIOC, LED0_Pin|LED1_Pin, GPIO_PIN_RESET);
    while (readflag) {
        error = f_read (&fnew, rData, PageSize, &fnum);
        {
            printf ("f_read = %d\r\n", fnum);
            if (error)
                return 88;
            if (fnum < PageSize) {
                readflag = 0;
            }
        }
        error = target_flash_program_page (BeginAddr + addr, (uint8_t *)&rData[0], PageSize);
        if (error)
            return 8;
        addr += PageSize;
        // readflag--;
        printf ("readflag = %d\r\n", readflag);
        // HAL_GPIO_TogglePin(GPIOC,LED0_Pin);
    }
    f_close (&fnew);  // 读取完毕，关闭文件。
    // HAL_GPIO_WritePin(GPIOC, LED0_Pin|LED1_Pin, GPIO_PIN_SET);

    // step 9
    error = swd_init_debug();
    if (!error)
        return 9;
    // step 10
    swd_set_target_reset (0);  // 复位运行
    printf ("dap_offline_download download ok\r\n");
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
