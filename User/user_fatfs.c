#include "user_fatfs.h"
#include "string.h"
#include "my_data_array.h"
#include "dap_flash.h"
FATFS fs;
FIL fnew;
const TCHAR* SetupPath = "setup.ry";

void FatReadDirTest (uint8_t flag, char *FilePath) {
    DIR fnew;  // ����������ΪDIR���ͣ�Ŀ¼����
    FILINFO FilInfo;

    // ������ȷ��fnew��DIR���ͣ�path��Ϊ�ڶ�������
    if (f_opendir (&fnew, "0:") == FR_OK) {
        while (f_readdir (&fnew, &FilInfo) == FR_OK) {
            if (!FilInfo.fname[0])
                break;
            printf ("fname:%s\r\n", FilInfo.fname);
            if (strstr (FilInfo.fname, "BIN") && flag) {
                // �����ļ������Ż��棩
                strncpy (FilePath, FilInfo.fname, 10);
                FilePath[12] = '\0';  // ȷ���ַ�����ֹ
                flag--;
            }
        }
        f_closedir (&fnew);  // �ر�Ŀ¼��������Դй©��
    }
}
#include "bsp_spi_flash.h"
static uint8_t work_buf[4096] __attribute__((aligned(4)));  // 8KB��̬������
void fatfs_file_init (void) {
    UINT fnum;
    // u8 WriteBuf[] = {"Hello WCH!"};
    u8 ReadSDBuf[11] = {0};
    Flash_Init();
    Flash_SectorErase(0);

    FRESULT res_sd = f_mount (&fs, "0:", 1);  // SD mount
    if (res_sd == FR_OK) {
        printf ("Disk mounted successfully\r\n");
    }
    if (res_sd == FR_NO_FILESYSTEM) {
        printf ("Disk formatting\r\n");
        res_sd = f_mkfs("0:", 0, work_buf, sizeof(work_buf));
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

    res_sd = f_open (&fnew, "f103.bin", FA_CREATE_ALWAYS | FA_WRITE);
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
    res_sd = f_open (&fnew, "f103.bin", FA_OPEN_EXISTING | FA_READ);
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

    // 32bit data write/read test

    res_sd = f_open (&fnew, SetupPath, FA_CREATE_ALWAYS | FA_WRITE);
    printf ("Writing......................\r\n");

    // 1.д�����ݳ���
    uint32_t datatemp = sizeof (flash_code1);
    res_sd = f_write (&fnew, &datatemp, sizeof (datatemp), &fnum);
    printf ("write size:%d bytes\r\n", fnum);

    // 2.����д����������flash_code
    res_sd = f_write (&fnew, flash_code1, sizeof (flash_code1), &fnum);
    printf ("write data:%d bytes\r\n", fnum);
    // 3.д��program_target_t flash_algo_F10x
    res_sd = f_write (&fnew, (uint32_t*)&flash_algo_F429, sizeof (flash_algo_F429), &fnum);
    printf ("write data:%d bytes\r\n", fnum);
    
    f_close (&fnew);
}

uint8_t load_setup(void)
{
    UINT fnum;
    FRESULT res_sd = f_open (&fnew,SetupPath, FA_OPEN_EXISTING | FA_READ);
    if (res_sd != FR_OK) 
    {   
        printf ("Open Setup.ry error\r\n");
        return 1;
    }

    // 1.��ȡ���ݳ���
    uint32_t readtemp=0;
    res_sd = f_read (&fnew, &readtemp, sizeof(readtemp), (UINT *)&fnum);//printf ("readtemp=0x%08X\r\n", readtemp);
    if (res_sd != FR_OK) 
    {   
        printf ("read size error\r\n");
        return 2;
    }

    // 2.������ȡ���ݴ洢��flash_code
    res_sd = f_read (&fnew, flash_code, readtemp, (UINT *)&fnum);
    if (res_sd != FR_OK) 
    {   
        printf ("read flash_code error\r\n");
        return 3;
    }

    // 3.������д��program_target_t flash_algo_F10x
    uint32_t* ptemp = (uint32_t*)&flash_algo_F10x;
    res_sd = f_read (&fnew, ptemp, sizeof(flash_algo_F10x), (UINT *)&fnum);
    flash_algo_F10x.algo_blob = flash_code;
    if (res_sd != FR_OK) 
    {   
        printf ("read flash_algo_F10x error\r\n");
        return 34;
    }

    f_close (&fnew); 

    return 0;   
}