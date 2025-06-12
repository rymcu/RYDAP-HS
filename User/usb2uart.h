#ifndef __USB2UART_H
#define __USB2UART_H

#include "ch32v30x.h"
#include "dap_main.h"
#include "usb2uart.h"

#define UART_TX_PIN 11
#define UART_RX_PIN 13

#define UART_RX_DMA_BUF_SIZE (16 * 1024)

void uartx_preinit(void);

#endif // __USB2UART_H
