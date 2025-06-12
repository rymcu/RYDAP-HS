#include "usbd_core.h"
#include "usb_ch32_usbhs_reg.h"

#ifndef USBD_IRQHandler
#define USBD_IRQHandler USBHS_IRQHandler //use actual usb irq name instead
#endif

#ifndef USB_NUM_BIDIR_ENDPOINTS
#define USB_NUM_BIDIR_ENDPOINTS 16
#endif

#define USB_SET_RX_DMA(ep_idx, addr) (*(volatile uint32_t *)((uint32_t)(&USBHS_DEVICE->UEP1_RX_DMA) + 4 * (ep_idx - 1)) = addr)
#define USB_SET_TX_DMA(ep_idx, addr) (*(volatile uint32_t *)((uint32_t)(&USBHS_DEVICE->UEP1_TX_DMA) + 4 * (ep_idx - 1)) = addr)
#define USB_SET_MAX_LEN(ep_idx, len) (*(volatile uint16_t *)((uint32_t)(&USBHS_DEVICE->UEP0_MAX_LEN) + 4 * ep_idx) = len)
#define USB_SET_TX_LEN(ep_idx, len)  (*(volatile uint16_t *)((uint32_t)(&USBHS_DEVICE->UEP0_TX_LEN) + 4 * ep_idx) = len)
#define USB_GET_TX_LEN(ep_idx)       (*(volatile uint16_t *)((uint32_t)(&USBHS_DEVICE->UEP0_TX_LEN) + 4 * ep_idx))
#define USB_SET_TX_CTRL(ep_idx, val) (*(volatile uint8_t *)((uint32_t)(&USBHS_DEVICE->UEP0_TX_CTRL) + 4 * ep_idx) = val)
#define USB_GET_TX_CTRL(ep_idx)      (*(volatile uint8_t *)((uint32_t)(&USBHS_DEVICE->UEP0_TX_CTRL) + 4 * ep_idx))
#define USB_SET_RX_CTRL(ep_idx, val) (*(volatile uint8_t *)((uint32_t)(&USBHS_DEVICE->UEP0_RX_CTRL) + 4 * ep_idx) = val)
#define USB_GET_RX_CTRL(ep_idx)      (*(volatile uint8_t *)((uint32_t)(&USBHS_DEVICE->UEP0_RX_CTRL) + 4 * ep_idx))

/* Endpoint state */
struct ch32_usbhs_ep_state {
    uint16_t ep_mps;    /* Endpoint max packet size */
    uint8_t ep_type;    /* Endpoint type */
    uint8_t ep_stalled; /* Endpoint stall flag */
    uint8_t ep_enable;  /* Endpoint enable */
    uint8_t *xfer_buf;
    uint32_t xfer_len;
    uint32_t actual_xfer_len;
};

/* Driver state */
struct ch32_usbhs_udc {
    __attribute__((aligned(4))) struct usb_setup_packet setup;
    volatile uint8_t dev_addr;
    struct ch32_usbhs_ep_state in_ep[USB_NUM_BIDIR_ENDPOINTS];  /*!< IN endpoint parameters*/
    struct ch32_usbhs_ep_state out_ep[USB_NUM_BIDIR_ENDPOINTS]; /*!< OUT endpoint parameters */
} g_ch32_usbhs_udc;

void USBHS_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

volatile uint8_t mps_over_flag = 0;
volatile bool ep0_rx_data_toggle;
volatile bool ep0_tx_data_toggle;
volatile bool epx_tx_data_toggle[USB_NUM_BIDIR_ENDPOINTS - 1];

__WEAK void usb_dc_low_level_init(void)
{
}

__WEAK void usb_dc_low_level_deinit(void)
{
}

int usb_dc_init(void)
{
    usb_dc_low_level_init();

    USBHS_DEVICE->HOST_CTRL = 0x00;
    USBHS_DEVICE->HOST_CTRL = USBHS_PHY_SUSPENDM;

    USBHS_DEVICE->CONTROL = 0;
#ifdef CONFIG_USB_HS
    USBHS_DEVICE->CONTROL = USBHS_DMA_EN | USBHS_INT_BUSY_EN | USBHS_HIGH_SPEED;
#else
    USBHS_DEVICE->CONTROL = USBHS_DMA_EN | USBHS_INT_BUSY_EN | USBHS_FULL_SPEED;
#endif

    USBHS_DEVICE->INT_FG = 0xff;
    USBHS_DEVICE->INT_EN = 0;
    USBHS_DEVICE->INT_EN = USBHS_SETUP_ACT_EN | USBHS_TRANSFER_EN | USBHS_DETECT_EN;

    /* ALL endpoint enable */
    USBHS_DEVICE->ENDP_CONFIG = 0xffffffff;

    USBHS_DEVICE->ENDP_TYPE = 0x00;
    USBHS_DEVICE->BUF_MODE = 0x00;

    USBHS_DEVICE->CONTROL |= USBHS_DEV_PU_EN;

    return 0;
}

int usb_dc_deinit(void)
{
    return 0;
}

int usbd_set_address(const uint8_t addr)
{
    if (addr == 0) {
        USBHS_DEVICE->DEV_AD = addr & 0xff;
    }
    g_ch32_usbhs_udc.dev_addr = addr;
    return 0;
}

uint8_t usbd_get_port_speed(const uint8_t port)
{
    return USB_SPEED_HIGH;
}

int usbd_ep_open(const struct usbd_endpoint_cfg *ep_cfg)
{
    uint8_t ep_idx = USB_EP_GET_IDX(ep_cfg->ep_addr);

    if (USB_EP_DIR_IS_OUT(ep_cfg->ep_addr)) {
        g_ch32_usbhs_udc.out_ep[ep_idx].ep_mps = ep_cfg->ep_mps;
        g_ch32_usbhs_udc.out_ep[ep_idx].ep_type = ep_cfg->ep_type;
        g_ch32_usbhs_udc.out_ep[ep_idx].ep_enable = true;
        USBHS_DEVICE->ENDP_CONFIG |= (1 << (ep_idx + 16));
        USB_SET_RX_CTRL(ep_idx, USBHS_EP_R_RES_NAK | USBHS_EP_R_TOG_0 | USBHS_EP_R_AUTOTOG);
    } else {
        g_ch32_usbhs_udc.in_ep[ep_idx].ep_mps = ep_cfg->ep_mps;
        g_ch32_usbhs_udc.in_ep[ep_idx].ep_type = ep_cfg->ep_type;
        g_ch32_usbhs_udc.in_ep[ep_idx].ep_enable = true;
        USBHS_DEVICE->ENDP_CONFIG |= (1 << (ep_idx));
        USB_SET_TX_CTRL(ep_idx, USBHS_EP_T_RES_NAK | USBHS_EP_T_TOG_0 | USBHS_EP_T_AUTOTOG);
    }
    USB_SET_MAX_LEN(ep_idx, ep_cfg->ep_mps);
    return 0;
}

int usbd_ep_close(const uint8_t ep)
{
    return 0;
}

int usbd_ep_set_stall(const uint8_t ep)
{
    uint8_t ep_idx = USB_EP_GET_IDX(ep);

    if (USB_EP_DIR_IS_OUT(ep)) {
        if (ep_idx == 0) {
            USBHS_DEVICE->UEP0_RX_CTRL = USBHS_EP_R_RES_STALL;
        } else {
            USB_SET_RX_CTRL(ep_idx, (USB_GET_RX_CTRL(ep_idx) & ~USBHS_EP_R_RES_MASK) | USBHS_EP_R_RES_STALL);
        }
    } else {
        if (ep_idx == 0) {
            USBHS_DEVICE->UEP0_TX_CTRL = USBHS_EP_T_RES_STALL;
        } else {
            USB_SET_TX_CTRL(ep_idx, (USB_GET_TX_CTRL(ep_idx) & ~USBHS_EP_T_RES_MASK) | USBHS_EP_T_RES_STALL);
        }
    }

    return 0;
}

int usbd_ep_clear_stall(const uint8_t ep)
{
    uint8_t ep_idx = USB_EP_GET_IDX(ep);

    if (USB_EP_DIR_IS_OUT(ep)) {
        USB_SET_RX_CTRL(ep_idx, USBHS_EP_R_RES_ACK | USBHS_EP_R_TOG_0);
    } else {
        USB_SET_TX_CTRL(ep_idx, USBHS_EP_T_RES_NAK | USBHS_EP_T_TOG_0);
    }
    return 0;
}

int usbd_ep_is_stalled(const uint8_t ep, uint8_t *stalled)
{
    return 0;
}

int usbd_ep_start_write(const uint8_t ep, const uint8_t *data, uint32_t data_len)
{
    uint8_t ep_idx = USB_EP_GET_IDX(ep);
    uint32_t tmp;

    if (!data && data_len) {
        return -1;
    }
    if (!g_ch32_usbhs_udc.in_ep[ep_idx].ep_enable) {
        return -2;
    }
    if ((uint32_t)data & 0x03) {
        return -3;
    }

    g_ch32_usbhs_udc.in_ep[ep_idx].xfer_buf = (uint8_t *)data;
    g_ch32_usbhs_udc.in_ep[ep_idx].xfer_len = data_len;
    g_ch32_usbhs_udc.in_ep[ep_idx].actual_xfer_len = 0;

    if (ep_idx == 0) {
        if (data_len == 0) {
            USB_SET_TX_LEN(ep_idx, 0);
        } else {
            data_len = MIN(data_len, g_ch32_usbhs_udc.in_ep[ep_idx].ep_mps);
            USB_SET_TX_LEN(ep_idx, data_len);
            USBHS_DEVICE->UEP0_DMA = (uint32_t)data;
        }
        tmp = ep0_tx_data_toggle ? USBHS_EP_T_TOG_1 : USBHS_EP_T_TOG_0;
        USBHS_DEVICE->UEP0_TX_CTRL = USBHS_EP_T_RES_ACK | tmp;
    } else {
        if (data_len == 0) {
            USB_SET_TX_LEN(ep_idx, 0);
        } else {
            data_len = MIN(data_len, g_ch32_usbhs_udc.in_ep[ep_idx].ep_mps);
            USB_SET_TX_LEN(ep_idx, data_len);
            USB_SET_TX_DMA(ep_idx, (uint32_t)data);
        }
        tmp = USB_GET_TX_CTRL(ep_idx);
        tmp &= ~(USBHS_EP_T_RES_MASK | USBHS_EP_T_TOG_MASK);
        tmp |= USBHS_EP_T_RES_ACK;
        tmp |= (epx_tx_data_toggle[ep_idx - 1] ? USBHS_EP_T_TOG_1 : USBHS_EP_T_TOG_0);
        USB_SET_TX_CTRL(ep_idx, tmp);
    }
    return 0;
}

int usbd_ep_start_read(const uint8_t ep, uint8_t *data, uint32_t data_len)
{
    uint8_t ep_idx = USB_EP_GET_IDX(ep);

    if (!data && data_len) {
        return -1;
    }
    if (!g_ch32_usbhs_udc.out_ep[ep_idx].ep_enable) {
        return -2;
    }
    if ((uint32_t)data & 0x03) {
        return -3;
    }

    g_ch32_usbhs_udc.out_ep[ep_idx].xfer_buf = (uint8_t *)data;
    g_ch32_usbhs_udc.out_ep[ep_idx].xfer_len = data_len;
    g_ch32_usbhs_udc.out_ep[ep_idx].actual_xfer_len = 0;

    if (ep_idx == 0) {
        if (data_len == 0) {
            USBHS_DEVICE->UEP0_RX_CTRL = USBHS_EP_R_RES_ACK | USBHS_EP_R_TOG_1;
        } else {
            USBHS_DEVICE->UEP0_DMA = (uint32_t)data;
            USBHS_DEVICE->UEP0_RX_CTRL = USBHS_EP_R_RES_ACK | (ep0_rx_data_toggle ? USBHS_EP_R_TOG_1 : USBHS_EP_R_TOG_0);
        }
        return 0;
    } else {
        USB_SET_RX_DMA(ep_idx, (uint32_t)data);
        USB_SET_RX_CTRL(ep_idx, (USB_GET_RX_CTRL(ep_idx) & ~USBHS_EP_R_RES_MASK) | USBHS_EP_R_RES_ACK);
    }

    return 0;
}

void USBD_IRQHandler(void)
{
    uint32_t ep_idx, token, write_count, read_count;
    uint8_t intflag = 0;

    intflag = USBHS_DEVICE->INT_FG;//1.读中断标志寄存器，获取中断类型（传输完成/设置请求/连接检测）

    if (intflag & USBHS_TRANSFER_FLAG) //2.传输完成
    {
        ep_idx = (USBHS_DEVICE->INT_ST) & MASK_UIS_ENDP;
        token = (((USBHS_DEVICE->INT_ST) & MASK_UIS_TOKEN) >> 4) & 0x03;

        if (token == PID_IN)//2.1 IN 令牌处理（主机读取数据）
        {
            if (ep_idx == 0x00) //2.1.1 端点0数据
            {
                //数据没发送完继续发送
                if (g_ch32_usbhs_udc.in_ep[ep_idx].xfer_len > g_ch32_usbhs_udc.in_ep[ep_idx].ep_mps) {
                    g_ch32_usbhs_udc.in_ep[ep_idx].xfer_len -= g_ch32_usbhs_udc.in_ep[ep_idx].ep_mps;
                    g_ch32_usbhs_udc.in_ep[ep_idx].actual_xfer_len += g_ch32_usbhs_udc.in_ep[ep_idx].ep_mps;
                    ep0_tx_data_toggle ^= 1;
                } else 
                {
                    g_ch32_usbhs_udc.in_ep[ep_idx].actual_xfer_len += g_ch32_usbhs_udc.in_ep[ep_idx].xfer_len;
                    g_ch32_usbhs_udc.in_ep[ep_idx].xfer_len = 0;
                    ep0_tx_data_toggle = true;
                }

                usbd_event_ep_in_complete_handler(ep_idx | 0x80, g_ch32_usbhs_udc.in_ep[ep_idx].actual_xfer_len);

                if (g_ch32_usbhs_udc.dev_addr > 0) {//设置地址阶段，设置完了清零，防止重复设置
                    USBHS_DEVICE->DEV_AD = g_ch32_usbhs_udc.dev_addr & 0xff;
                    g_ch32_usbhs_udc.dev_addr = 0;
                }
                //判断是否包含数据阶段，并且方向为主机到设备（OUT），则指定ep0 dma接收地址，并允许中断
                if (g_ch32_usbhs_udc.setup.wLength && ((g_ch32_usbhs_udc.setup.bmRequestType & USB_REQUEST_DIR_MASK) == USB_REQUEST_DIR_OUT)) {
                    /* In status, start reading setup */
                    USBHS_DEVICE->UEP0_DMA = (uint32_t)&g_ch32_usbhs_udc.setup;
                    USBHS_DEVICE->UEP0_RX_CTRL = USBHS_EP_R_RES_ACK;
                    ep0_tx_data_toggle = true;

                } else if (g_ch32_usbhs_udc.setup.wLength == 0) {//无数据阶段处理方式一样
                    /* In status, start reading setup */
                    USBHS_DEVICE->UEP0_DMA = (uint32_t)&g_ch32_usbhs_udc.setup;
                    USBHS_DEVICE->UEP0_RX_CTRL = USBHS_EP_R_RES_ACK;
                    ep0_tx_data_toggle = true;
                }
            } else//2.1.2 非端点0数据
             {
                USB_SET_TX_CTRL(ep_idx, (USB_GET_TX_CTRL(ep_idx) & ~(USBHS_EP_T_RES_MASK | USBHS_EP_T_TOG_MASK)) | USBHS_EP_T_RES_NAK | USBHS_EP_T_TOG_0);

                if (g_ch32_usbhs_udc.in_ep[ep_idx].xfer_len > g_ch32_usbhs_udc.in_ep[ep_idx].ep_mps) {
                    g_ch32_usbhs_udc.in_ep[ep_idx].xfer_buf += g_ch32_usbhs_udc.in_ep[ep_idx].ep_mps;
                    g_ch32_usbhs_udc.in_ep[ep_idx].xfer_len -= g_ch32_usbhs_udc.in_ep[ep_idx].ep_mps;
                    g_ch32_usbhs_udc.in_ep[ep_idx].actual_xfer_len += g_ch32_usbhs_udc.in_ep[ep_idx].ep_mps;
                    epx_tx_data_toggle[ep_idx - 1] ^= 1;

                    write_count = MIN(g_ch32_usbhs_udc.in_ep[ep_idx].xfer_len, g_ch32_usbhs_udc.in_ep[ep_idx].ep_mps);
                    USB_SET_TX_LEN(ep_idx, write_count);
                    USB_SET_TX_DMA(ep_idx, (uint32_t)g_ch32_usbhs_udc.in_ep[ep_idx].xfer_buf);

                    uint32_t tmp = USB_GET_TX_CTRL(ep_idx);
                    tmp &= ~(USBHS_EP_T_RES_MASK | USBHS_EP_T_TOG_MASK);
                    tmp |= USBHS_EP_T_RES_ACK;
                    tmp |= (epx_tx_data_toggle[ep_idx - 1] ? USBHS_EP_T_TOG_1 : USBHS_EP_T_TOG_0);
                    USB_SET_TX_CTRL(ep_idx, tmp);
                } else {
                    g_ch32_usbhs_udc.in_ep[ep_idx].actual_xfer_len += g_ch32_usbhs_udc.in_ep[ep_idx].xfer_len;
                    g_ch32_usbhs_udc.in_ep[ep_idx].xfer_len = 0;
                    epx_tx_data_toggle[ep_idx - 1] ^= 1;
                    usbd_event_ep_in_complete_handler(ep_idx | 0x80, g_ch32_usbhs_udc.in_ep[ep_idx].actual_xfer_len);
                }
            }
        } else if (token == PID_OUT) //2.2 OUT 令牌处理（主机发送数据）
        {
            if (ep_idx == 0x00) {
                read_count = USBHS_DEVICE->RX_LEN;

                g_ch32_usbhs_udc.out_ep[ep_idx].actual_xfer_len += read_count;
                g_ch32_usbhs_udc.out_ep[ep_idx].xfer_len -= read_count;

                usbd_event_ep_out_complete_handler(0x00, g_ch32_usbhs_udc.out_ep[ep_idx].actual_xfer_len);

                if (read_count == 0) {
                    /* Out status, start reading setup */
                    USBHS_DEVICE->UEP0_DMA = (uint32_t)&g_ch32_usbhs_udc.setup;
                    USBHS_DEVICE->UEP0_RX_CTRL = USBHS_EP_R_RES_ACK;
                    ep0_rx_data_toggle = true;
                    ep0_tx_data_toggle = true;
                } else {
                    ep0_rx_data_toggle ^= 1;
                }
            } else {
                if (USBHS_DEVICE->INT_ST & USBHS_DEV_UIS_TOG_OK) {
                    USB_SET_RX_CTRL(ep_idx, (USB_GET_RX_CTRL(ep_idx) & ~USBHS_EP_R_RES_MASK) | USBHS_EP_R_RES_NAK);
                    read_count = USBHS_DEVICE->RX_LEN;

                    g_ch32_usbhs_udc.out_ep[ep_idx].xfer_buf += read_count;
                    g_ch32_usbhs_udc.out_ep[ep_idx].actual_xfer_len += read_count;
                    g_ch32_usbhs_udc.out_ep[ep_idx].xfer_len -= read_count;

                    if ((read_count < g_ch32_usbhs_udc.out_ep[ep_idx].ep_mps) || (g_ch32_usbhs_udc.out_ep[ep_idx].xfer_len == 0)) {
                        usbd_event_ep_out_complete_handler(ep_idx, g_ch32_usbhs_udc.out_ep[ep_idx].actual_xfer_len);
                    } else {
                        USB_SET_RX_DMA(ep_idx, (uint32_t)g_ch32_usbhs_udc.out_ep[ep_idx].xfer_buf);
                        USB_SET_RX_CTRL(ep_idx, (USB_GET_RX_CTRL(ep_idx) & ~USBHS_EP_R_RES_MASK) | USBHS_EP_R_RES_ACK);
                    }
                }
            }
        }
        USBHS_DEVICE->INT_FG = USBHS_TRANSFER_FLAG;//清除中断标志
    } else if (intflag & USBHS_SETUP_FLAG) //3.设置请求
    {
        usbd_event_ep0_setup_complete_handler((uint8_t *)&g_ch32_usbhs_udc.setup);//当接收到设置包（SETUP 事务）时，触发回调函数处理 USB 请求（如获取描述符、设置地址等）
        USBHS_DEVICE->INT_FG = USBHS_SETUP_FLAG;//清除中断标志位
    } else if (intflag & USBHS_DETECT_FLAG)//4.连接检测
     {
        USBHS_DEVICE->ENDP_CONFIG = USBHS_EP0_T_EN | USBHS_EP0_R_EN;//启动EP0接收和发送通道

        USBHS_DEVICE->UEP0_TX_LEN = 0;
        USBHS_DEVICE->UEP0_TX_CTRL = USBHS_EP_T_RES_NAK;//EP0发送设置为NAK，长度设置为0（初始化阶段不主动发送数据，需要等待主机请求）

        ep0_tx_data_toggle = true;//重置EP0的发送接收toggle位，通常为DATA0
        ep0_rx_data_toggle = true;

        for (uint8_t ep_idx = 1; ep_idx < USB_NUM_BIDIR_ENDPOINTS; ep_idx++) //初始化端点，禁用发送和接收功能（在枚举完成前，禁用所有非EP0端点）
        {
            USB_SET_TX_LEN(ep_idx, 0);
            USB_SET_TX_CTRL(ep_idx, USBHS_EP_T_AUTOTOG | USBHS_EP_T_RES_NAK); // autotog does not work
            USB_SET_RX_CTRL(ep_idx, USBHS_EP_R_AUTOTOG | USBHS_EP_R_RES_NAK);
            epx_tx_data_toggle[ep_idx - 1] = false;
        }

        memset(&g_ch32_usbhs_udc, 0, sizeof(struct ch32_usbhs_udc));//重置USB设备
        usbd_event_reset_handler();//通知上层应用，设备已复位
        USBHS_DEVICE->UEP0_DMA = (uint32_t)&g_ch32_usbhs_udc.setup;//将 EP0 的 DMA 缓冲区地址指向设置请求结构体（g_ch32_usbhs_udc.setup）
        USBHS_DEVICE->UEP0_RX_CTRL = USBHS_EP_R_RES_ACK;//将 EP0 接收控制寄存器设为 ACK 状态，允许接收数据
        USBHS_DEVICE->INT_FG = USBHS_DETECT_FLAG;//清除 USB 检测中断标志，结束当前中断处理
    }
}
