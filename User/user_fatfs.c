#include "user_fatfs.h"
#include "string.h"
#include "my_data_array.h"

FATFS fs;
FIL fnew;

void FatReadDirTest (uint8_t flag, char *FilePath) {
    DIR fnew;  // 修正：声明为DIR类型（目录对象）
    FILINFO FilInfo;

    // 修正：确保fnew是DIR类型，path作为第二个参数
    if (f_opendir (&fnew, "0:") == FR_OK) {
        while (f_readdir (&fnew, &FilInfo) == FR_OK) {
            if (!FilInfo.fname[0])
                break;
            printf ("fname:%s\r\n", FilInfo.fname);
            if (strstr (FilInfo.fname, "BIN") && flag) {
                // 复制文件名（优化版）
                strncpy (FilePath, FilInfo.fname, 10);
                FilePath[12] = '\0';  // 确保字符串终止
                flag--;
            }
        }
        f_closedir (&fnew);  // 关闭目录（避免资源泄漏）
    }
}

void fatfs_file_init (void) {
    UINT fnum;
    // u8 WriteBuf[] = {"Hello WCH!"};
    u8 ReadSDBuf[11] = {0};
    FRESULT res_sd = f_mount (&fs, "0:", 1);  // SD mount
    if (res_sd == FR_OK) {
        printf ("Disk mounted successfully\r\n");
    }
    if (res_sd == FR_NO_FILESYSTEM) {
        printf ("Disk formatting\r\n");
        res_sd = f_mkfs ("0:", NULL, NULL, 0);
        if (res_sd == FR_OK) {
            printf ("Disk formatted successfully\r\n");
            res_sd = f_mount (&fs, "0:", 1);
            if (res_sd == FR_OK) {
                printf ("Disk mounted successfully\r\n");
            } else {
                printf ("Disk mounting failed\r\n");
                printf ("error code%x\r\n", res_sd);
            }
        } else {
            printf ("Disk formatting failed!(%d)\r\n", res_sd);
        }
    }
    res_sd = f_open (&fnew, (const char *)"0:f103.bin", FA_CREATE_ALWAYS | FA_WRITE);
    if (res_sd != FR_OK) {
        printf ("Create file error\r\n");
    } else {
        printf ("Create file successfully\r\n");
    }
    printf ("Writing......................\r\n");
    res_sd = f_write (&fnew, my_data_array, sizeof (my_data_array), &fnum);
    printf ("write data:%d)\r\n", fnum);
    // for (int j = 0; j < sizeof (my_data_array); j++) {
    //     printf ("%02X ", my_data_array[j]);
    // }
    if (res_sd != FR_OK) {
        printf ("Write error\r\n");
    }
    f_close (&fnew);
    res_sd = f_open (&fnew, (const char *)"0:f103.bin", FA_OPEN_EXISTING | FA_READ);
    if (res_sd != FR_OK) {
        printf ("Open file error\r\n");
    }
    printf ("Reading......................\r\n");
    res_sd = f_read (&fnew, ReadSDBuf, 11, (UINT *)&fnum);
    if (res_sd != FR_OK) {
        printf ("Read error\r\n");
    }
    printf ("read data:%d)\r\n", fnum);
    // for (int i = 0; i < sizeof (ReadSDBuf); i++) {
    //     printf ("ReadBuf[%d]=%02X\r\n", i, ReadSDBuf[i]);
    // }
    f_close (&fnew);
}
