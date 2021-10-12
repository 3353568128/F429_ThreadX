#include "SDIO.h"
#include "main.h"
#include "string.h"

extern SD_HandleTypeDef hsd;

/*读取SD信息
 *返回值：扇区数量
 *SD容量 = 扇区数量 * 512KB
 */
uint32_t SD_Check(void)
{
	HAL_SD_CardCSDTypeDef SDCSD;
	HAL_SD_GetCardCSD(&hsd, &SDCSD);
	while(HAL_SD_GetCardState(&hsd) != HAL_SD_CARD_TRANSFER);
	
//	uint8_t SD_ReadBuffer[512];
//	memset(SD_ReadBuffer,  0, 512);
//	HAL_SD_ReadBlocks(&hsd, SD_ReadBuffer, 0, 1,10);
//	while(HAL_SD_GetCardState(&hsd) != HAL_SD_CARD_TRANSFER);
	
	return SDCSD.DeviceSize;
}


void SD_ReadDisk(uint8_t*buf,uint32_t sector,uint8_t cnt)
{

	HAL_SD_ReadBlocks(&hsd, buf, sector, cnt,10);
	while(HAL_SD_GetCardState(&hsd) != HAL_SD_CARD_TRANSFER);
}

void SD_WriteDisk(uint8_t*buf,uint32_t sector,uint8_t cnt)
{
	HAL_SD_WriteBlocks(&hsd, buf, sector, cnt,10);
	while(HAL_SD_GetCardState(&hsd) != HAL_SD_CARD_TRANSFER);
}	