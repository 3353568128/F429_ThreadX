#ifndef __SDIO_H__
#define __SDIO_H__
#include "main.h"

uint32_t SD_Check(void);
void SD_ReadDisk(uint8_t*buf,uint32_t sector,uint8_t cnt);
void SD_WriteDisk(uint8_t*buf,uint32_t sector,uint8_t cnt);

#endif