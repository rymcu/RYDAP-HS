#include "OffLineDAP.h"

#define PageSize  1024
uint8_t OffLineDAP(FATFS fs,FIL fnew,char *path,uint32_t McuType)
{
    uint8_t error,rData[PageSize];
    uint32_t readflag = 99;
    uint32_t BeginAddr,addr = 0;
    UINT fnum;
    BeginAddr = 0x08000000;
    //PageSize  = 1024;
    //FATFS fs;//文件系统对象
    //static FIL fnew;//文件对象

    Findflash(McuType);//选择对对应的MCU型号
    for (int i = 0; i < PageSize; ++i) {rData[i] = 0x55;}
    //step 1
    error = swd_init_debug(); if(!error) return 5;
    //step 2
    error = target_flash_init(BeginAddr); if(error) return 6;
    //step 3
    error = target_flash_erase_chip();if(error) return 7;
    //setp 4

    error = f_open(&fnew,path,FA_OPEN_EXISTING | FA_READ);
    if(error) return 77;
    //printf("download：%s(%0.1fK)\r\n", path,f_size(&fnew)/1024.0);
    //HAL_GPIO_WritePin(GPIOC, LED0_Pin|LED1_Pin, GPIO_PIN_RESET);
    while(readflag)
    {
        error = f_read(&fnew,rData, PageSize,&fnum);if(error) return 88;if(fnum <PageSize){readflag = 0;}
        error = target_flash_program_page(BeginAddr + addr, (uint8_t *)&rData[0], PageSize);if(error) return 8;
        addr += PageSize;
        //readflag--;
       // HAL_GPIO_TogglePin(GPIOC,LED0_Pin);
    }
    f_close(&fnew);//读取完毕，关闭文件。
    //HAL_GPIO_WritePin(GPIOC, LED0_Pin|LED1_Pin, GPIO_PIN_SET);

    //step 9
    error = swd_init_debug();if(!error) return 9;
    //step 10
    swd_set_target_reset(0);//复位运行
    return 0;


}


void EraseSectorTest(void)
{
    uint32_t error,BeginAddr = 0x08000000;
    uint32_t addr=0x08020000;

    Findflash(1);
    swd_init_debug();
    target_flash_init(BeginAddr);
    error = target_flash_erase_sector(addr);
    //经测试：
    //erase_sector函数单次擦除1页，其中addr为实际地址，或者实际地址减去开始地址
    //sector和page的关系
    //1*sector = N*page
    //怎么获得：sector size？
    //根据具体的型号计算sector大小及分布
    //获得要下载程序的大小以及开始地址，计算占用哪些sector,再擦除！
    //algo中program_buffer_size = PageSize
    //
    //printf("erase sector:0x%X,error = %d!\r\n",addr,error);
}

