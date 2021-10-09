#include "SDIO.h"
#include "main.h"
#include "string.h"

extern SD_HandleTypeDef hsd;

#define Test_BlockSize  512	 						//定义数据大小,SD块大小为512字节
#define	Test_Addr 	 		0
#define	NumOf_Blocks		1

uint8_t SD_Status ; 		 								//SD卡检测标志位
uint8_t SD_WriteBuffer[Test_BlockSize];	//	写数据数组
uint8_t SD_ReadBuffer[Test_BlockSize];	//	读数据数组

HAL_SD_CardCSDTypeDef SDCSD;
uint8_t BSP_SD_GetCardState(void);

void SD_Test(void)
{
	uint32_t status = 0;
	uint32_t i=0;
	
	memset(SD_WriteBuffer, 0, 512);
	memset(SD_ReadBuffer,  0, 512);
	for(i=0;i<Test_BlockSize;i++)
		SD_WriteBuffer[i] = i;
	
	status = HAL_SD_GetCardCSD(&hsd, &SDCSD);
	while(BSP_SD_GetCardState() != 0);
	
//	status = HAL_SD_Erase(&hsd, Test_Addr, NumOf_Blocks);
//	while(BSP_SD_GetCardState() != 0);
//	
//	status = HAL_SD_ReadBlocks(&hsd, SD_ReadBuffer, Test_Addr, NumOf_Blocks,10);
//	while(BSP_SD_GetCardState() != 0);
//	
//	status = HAL_SD_WriteBlocks(&hsd, SD_WriteBuffer, Test_Addr, NumOf_Blocks,10);
//	while(BSP_SD_GetCardState() != 0);
	
	status = HAL_SD_ReadBlocks(&hsd, SD_ReadBuffer, Test_Addr, NumOf_Blocks,10);
	while(BSP_SD_GetCardState() != 0);
}

uint8_t BSP_SD_GetCardState(void)
{
  return((HAL_SD_GetCardState(&hsd) == HAL_SD_CARD_TRANSFER ) ? 0 : 1);
}


void SD_ReadDisk(uint8_t*buf,uint32_t sector,uint8_t cnt)
{

	HAL_SD_ReadBlocks(&hsd, buf, sector, cnt,10);//
	while(BSP_SD_GetCardState() != 0);
}

void SD_WriteDisk(uint8_t*buf,uint32_t sector,uint8_t cnt)
{
	HAL_SD_WriteBlocks(&hsd, buf, sector, cnt,10);
	while(BSP_SD_GetCardState() != 0);
}	