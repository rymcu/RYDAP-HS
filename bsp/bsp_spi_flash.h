#ifndef __BSP_SPI_FLASH_H
#define __BSP_SPI_FLASH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "debug.h"

/* 硬件相关配置（根据实际引脚修改） */
#define SPI_FLASH_SPI SPI3

#define RCC_SPI_FLASH RCC_APB1Periph_SPI3
#define RCC_SPI_FLASH_DATA RCC_APB2Periph_GPIOB
#define RCC_SPI_FLASH_CS RCC_APB2Periph_GPIOE

#define SPI_FLASH_CLK_PIN GPIO_Pin_3
#define SPI_FLASH_CLK_PORT GPIOB
#define SPI_FLASH_DO_PIN GPIO_Pin_4
#define SPI_FLASH_DO_PORT GPIOB
#define SPI_FLASH_DI_PIN GPIO_Pin_5
#define SPI_FLASH_DI_PORT GPIOB
#define SPI_FLASH_CS_PIN GPIO_Pin_6
#define SPI_FLASH_CS_PORT GPIOE

/* W25Q64指令集 */
#define W25X_WriteEnable        0x06
#define W25X_WriteDisable       0x04
#define W25X_ReadStatusReg1     0x05
#define W25X_PageProgram        0x02
#define W25X_ReadData           0x03
#define W25X_SectorErase        0x20
#define W25X_ManufactDeviceID   0x90
#define W25X_JedecID            0x9F

#define FLASH_SECTOR_SIZE 4096
#define FLASH_SECTOR_COUNT 512
typedef enum
{
  HAL_OK       = 0x00U,
  HAL_ERROR    = 0x01U,
  HAL_BUSY     = 0x02U,
  HAL_TIMEOUT  = 0x03U
} HAL_StatusTypeDef;

/* 超时配置 */
#define FLASH_TIMEOUT_MS        5000  // 5秒超时

/* 函数声明 */
void Flash_Init(void);
void Flash_WriteEnable(void);
void Flash_WriteDisable(void);
uint8_t Flash_ReadStatusReg(void);
HAL_StatusTypeDef Flash_WaitBusy(void);
HAL_StatusTypeDef Flash_ReadData(uint32_t addr, uint8_t *pData, uint16_t Size);
void FLASH_WriteData(uint32_t addr,uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef Flash_PageProgram(uint32_t addr, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef Flash_SectorErase(uint32_t addr);
u16 Flash_ReadID(void);
u16 Flash_ReadJedecID(void);
 u8 SPI_FLASH_ReadWriteByte(u8 TxData);
#ifdef __cplusplus
}
#endif

#endif /* __BSP_SPI_FLASH_H */
