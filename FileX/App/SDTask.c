#include "SDTask.h"
#include "main.h"
#include "tx_api.h"
#include "fx_api.h"
#include "SDIO_drive.h"
#include "SDIO.h"
#include "UART.h"

#define SDTASK_POOL_SIZE	1024
static uint8_t SDTaskpool[SDTASK_POOL_SIZE];
static TX_THREAD SDTaskHand;

FX_MEDIA     sdio_disk; 
FX_FILE      fx_file;
uint32_t media_memory[1*1024];
char FsWriteBuf[1024] = {"1234567890\r\n"};
int status = 0;
static void SDTASK(ULONG thread_input)
{
	uint32_t SDofSize = 0;
	fx_system_initialize();
	
	status =  fx_media_open(&sdio_disk, "STM32_SDIO_DISK", F429_SD_SDIO_driver, 0, media_memory, sizeof(media_memory));
	status =  fx_file_create(&sdio_disk, "armfly.txt");
	status =  fx_file_open(&sdio_disk, &fx_file, "armfly.txt", FX_OPEN_FOR_WRITE);
	status =  fx_file_write(&fx_file, FsWriteBuf, strlen(FsWriteBuf));
	status =  fx_file_close(&fx_file);
	status =  fx_media_flush(&sdio_disk);
	
	while(1)
	{
		tx_thread_sleep(100);
		SDofSize = SD_Check() * 512 / 1024 / 1024;
		Printf((uint8_t *)"SD card of size: ", sizeof("SD card of size: "));
		PrintfDec(SDofSize);
		Printf((uint8_t *)"GB\r\n", sizeof("KB\r\n"));
	}
}

void SDTaskCreate()
{
	tx_thread_create(&SDTaskHand, "SD Task", SDTASK, NULL, SDTaskpool, SDTASK_POOL_SIZE, 3, 3, TX_NO_TIME_SLICE, TX_AUTO_START);
}