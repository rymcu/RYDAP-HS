#ifndef __SWD_OFFLINE_H__
#define __SWD_OFFLINE_H__

#include"DAP_config.h"
#include "DAP.h"
#include "swd_host.h"
#include "dap_flash.h"
#include "ff.h"
uint8_t dap_offline_download(FATFS fs,FIL fnew,char *path,uint32_t McuType);

#endif // __SWD_OFFLINE_H__
