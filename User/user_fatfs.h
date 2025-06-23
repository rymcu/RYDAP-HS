#ifndef __USER_FATFS_H
#define __USER_FATFS_H

#include "ff.h"
#include "debug.h"

extern FATFS fs;
extern FIL fnew;

void fatfs_file_init (void);
void FatReadDirTest (uint8_t flag,char* FilePath);

#endif /* __USER_FATFS_H */
