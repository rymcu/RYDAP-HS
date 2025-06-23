#include "bsp_spi_flash.h"
#include <string.h>

/* 私有宏 */
#define FLASH_CS_LOW() GPIO_WriteBit (SPI_FLASH_CS_PORT, SPI_FLASH_CS_PIN, 0);
#define FLASH_CS_HIGH() GPIO_WriteBit (SPI_FLASH_CS_PORT, SPI_FLASH_CS_PIN, 1);

/*********************** 驱动实现 ************************/

void Flash_Init (void) {
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    SPI_InitTypeDef SPI_InitStructure = {0};

    RCC_APB2PeriphClockCmd (RCC_SPI_FLASH_CS | RCC_SPI_FLASH_DATA, ENABLE);
    RCC_APB1PeriphClockCmd (RCC_SPI_FLASH, ENABLE);

    GPIO_InitStructure.GPIO_Pin = SPI_FLASH_CS_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init (SPI_FLASH_CS_PORT, &GPIO_InitStructure);
    GPIO_SetBits (SPI_FLASH_CS_PORT, SPI_FLASH_CS_PIN);

    GPIO_InitStructure.GPIO_Pin = SPI_FLASH_CLK_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init (SPI_FLASH_CLK_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = SPI_FLASH_DO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init (SPI_FLASH_DO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = SPI_FLASH_DI_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init (SPI_FLASH_DI_PORT, &GPIO_InitStructure);

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init (SPI_FLASH_SPI, &SPI_InitStructure);

    SPI_Cmd (SPI_FLASH_SPI, ENABLE);
}

void Flash_WriteEnable (void) {
    FLASH_CS_LOW();
    SPI_FLASH_ReadWriteByte (W25X_WriteEnable);
    FLASH_CS_HIGH();
}

void Flash_WriteDisable (void) {
    FLASH_CS_LOW();
    SPI_FLASH_ReadWriteByte (W25X_WriteDisable);
    FLASH_CS_HIGH();
}

uint8_t Flash_ReadStatusReg (void) {
    uint8_t status;
    FLASH_CS_LOW();
    SPI_FLASH_ReadWriteByte (W25X_ReadStatusReg1);
    status = SPI_FLASH_ReadWriteByte (0Xff);
    FLASH_CS_HIGH();
    return status;
}

HAL_StatusTypeDef Flash_WaitBusy (void) {
    while ((Flash_ReadStatusReg() & 0x01) == 0x01)
        ;
    return HAL_OK;
}

HAL_StatusTypeDef Flash_ReadData (uint32_t addr, uint8_t *pData, uint16_t Size) {
    u16 i;

    FLASH_CS_LOW();

    SPI_FLASH_ReadWriteByte (W25X_ReadData);
    SPI_FLASH_ReadWriteByte ((u8)((addr) >> 16));
    SPI_FLASH_ReadWriteByte ((u8)((addr) >> 8));
    SPI_FLASH_ReadWriteByte ((u8)addr);

    for (i = 0; i < Size; i++) {
        pData[i] = SPI_FLASH_ReadWriteByte (0XFF);
    }
    FLASH_CS_HIGH();
    return HAL_OK;
}
#define SPI_FLASH_PageSize 256
HAL_StatusTypeDef Flash_PageProgram (uint32_t addr, uint8_t *pData, uint16_t Size) {
    u16 i;
    /* 检查页边界 */
    if ((Size == 0) || (Size > SPI_FLASH_PageSize) || ((addr % SPI_FLASH_PageSize) + Size > SPI_FLASH_PageSize)) {
        return HAL_ERROR;
    }

    Flash_WriteEnable();
    FLASH_CS_LOW();
    SPI_FLASH_ReadWriteByte (W25X_PageProgram);
    SPI_FLASH_ReadWriteByte ((u8)((addr) >> 16));
    SPI_FLASH_ReadWriteByte ((u8)((addr) >> 8));
    SPI_FLASH_ReadWriteByte ((u8)addr);

    for (i = 0; i < Size; i++) {
        SPI_FLASH_ReadWriteByte (pData[i]);
    }
    FLASH_CS_HIGH();

    return Flash_WaitBusy();
}

HAL_StatusTypeDef Flash_SectorErase (uint32_t addr) {

    Flash_WriteEnable();
    Flash_WaitBusy();
    FLASH_CS_LOW();
    SPI_FLASH_ReadWriteByte (W25X_SectorErase);
    SPI_FLASH_ReadWriteByte ((u8)((addr) >> 16));
    SPI_FLASH_ReadWriteByte ((u8)((addr) >> 8));
    SPI_FLASH_ReadWriteByte ((u8)addr);
    FLASH_CS_HIGH();

    return Flash_WaitBusy();
}

u16 Flash_ReadID (void) {
    u16 Temp = 0;
    FLASH_CS_LOW();
    SPI_FLASH_ReadWriteByte (W25X_ManufactDeviceID);
    SPI_FLASH_ReadWriteByte (0x00);
    SPI_FLASH_ReadWriteByte (0x00);
    SPI_FLASH_ReadWriteByte (0x00);
    Temp |= SPI_FLASH_ReadWriteByte (0xFF) << 8;
    Temp |= SPI_FLASH_ReadWriteByte (0xFF);
    FLASH_CS_HIGH();

    return Temp;
}

u16 Flash_ReadJedecID (void) {
    u16 Temp = 0;
    FLASH_CS_LOW();
    SPI_FLASH_ReadWriteByte (W25X_JedecID);
    SPI_FLASH_ReadWriteByte (0x00);
    SPI_FLASH_ReadWriteByte (0x00);
    SPI_FLASH_ReadWriteByte (0x00);
    Temp |= SPI_FLASH_ReadWriteByte (0xFF) << 8;
    Temp |= SPI_FLASH_ReadWriteByte (0xFF);
    FLASH_CS_HIGH();

    return Temp;
}

/**
 * @brief  对FLASH写入数据，调用本函数写入数据前需要先擦除扇区
 * @param	pData，要写入数据的指针
 * @param  addr，写入地址
 * @param  Size，写入数据长度
 * @retval 无
 */


void FLASH_WriteData (uint32_t addr, uint8_t *pData, uint16_t Size) {
    uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

    /*mod运算求余，若writeAddr是SPI_FLASH_PageSize整数倍，运算结果Addr值为0*/
    Addr = addr % SPI_FLASH_PageSize;

    /*差count个数据值，刚好可以对齐到页地址*/
    count = SPI_FLASH_PageSize - Addr;
    /*计算出要写多少整数页*/
    NumOfPage = Size / SPI_FLASH_PageSize;
    /*mod运算求余，计算出剩余不满一页的字节数*/
    NumOfSingle = Size % SPI_FLASH_PageSize;

    /* Addr=0,则WriteAddr 刚好按页对齐 aligned  */
    if (Addr == 0) {
        /* Size < SPI_FLASH_PageSize */
        if (NumOfPage == 0) {
            Flash_PageProgram (addr, pData, Size);
        } else /* Size > SPI_FLASH_PageSize */
        {
            /*先把整数页都写了*/
            while (NumOfPage--) {
                Flash_PageProgram (addr, pData, SPI_FLASH_PageSize);
                addr += SPI_FLASH_PageSize;
                pData += SPI_FLASH_PageSize;
            }
            /*若有多余的不满一页的数据，把它写完*/
            Flash_PageProgram (addr, pData, NumOfSingle);
        }
    }
    /* 若地址与 SPI_FLASH_PageSize 不对齐  */
    else {
        /* Size < SPI_FLASH_PageSize */
        if (NumOfPage == 0) {
            /*当前页剩余的count个位置比NumOfSingle小，一页写不完*/
            if (NumOfSingle > count) {
                temp = NumOfSingle - count;
                /*先写满当前页*/
                Flash_PageProgram (addr, pData, count);

                addr += count;
                pData += count;
                /*再写剩余的数据*/
                Flash_PageProgram (addr, pData, temp);
            } else /*当前页剩余的count个位置能写完NumOfSingle个数据*/
            {
                Flash_PageProgram (addr, pData, Size);
            }
        } else /* NumByteToWrite > SPI_FLASH_PageSize */
        {
            /*地址不对齐多出的count分开处理，不加入这个运算*/
            Size -= count;
            NumOfPage = Size / SPI_FLASH_PageSize;
            NumOfSingle = Size % SPI_FLASH_PageSize;

            /* 先写完count个数据，为的是让下一次要写的地址对齐 */
            Flash_PageProgram (addr, pData, count);

            /* 接下来就重复地址对齐的情况 */
            addr += count;
            pData += count;
            /*把整数页都写了*/
            while (NumOfPage--) {
                Flash_PageProgram (addr, pData, SPI_FLASH_PageSize);
                addr += SPI_FLASH_PageSize;
                pData += SPI_FLASH_PageSize;
            }
            /*若有多余的不满一页的数据，把它写完*/
            if (NumOfSingle != 0) {
                Flash_PageProgram (addr, pData, NumOfSingle);
            }
        }
    }
}

/*********************** SPI底层传输 ************************/
u8 SPI_FLASH_ReadWriteByte (u8 TxData) {
    while (SPI_I2S_GetFlagStatus (SPI_FLASH_SPI, SPI_I2S_FLAG_TXE) == RESET)
        ;

    SPI_I2S_SendData (SPI_FLASH_SPI, TxData);

    while (SPI_I2S_GetFlagStatus (SPI_FLASH_SPI, SPI_I2S_FLAG_RXNE) == RESET)
        ;

    return SPI_I2S_ReceiveData (SPI_FLASH_SPI);
}