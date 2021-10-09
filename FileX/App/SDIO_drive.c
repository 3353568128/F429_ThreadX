#include "SDIO_drive.h"
#include "SDIO.h"
#include "main.h"
#include "tx_api.h"
#include "fx_api.h"

UINT  _fx_partition_offset_calculate(void  *partition_sector, UINT partition, ULONG *partition_start, ULONG *partition_size);
extern void MX_SDIO_SD_Init(void);

VOID  _fx_sd_spi_driver(FX_MEDIA *media_ptr)
{
	ULONG       boot_start;
	ULONG       boot_size;
	switch(media_ptr->fx_media_driver_request)
	{
		case FX_DRIVER_INIT:
		{
			MX_SDIO_SD_Init();
			media_ptr->fx_media_driver_status =  FX_SUCCESS;
			break;
		}
		
		case FX_DRIVER_UNINIT:
		{
			media_ptr->fx_media_driver_status =  FX_SUCCESS;
			break;
		}
		
		case FX_DRIVER_READ:
		{
			SD_ReadDisk((uint8_t*)media_ptr->fx_media_driver_buffer, media_ptr->fx_media_driver_logical_sector + media_ptr->fx_media_hidden_sectors, media_ptr->fx_media_driver_sectors);
			media_ptr->fx_media_driver_status =  FX_SUCCESS;
			break;
		}
		
		case FX_DRIVER_WRITE:
		{
			SD_WriteDisk((uint8_t*)media_ptr->fx_media_driver_buffer, media_ptr->fx_media_driver_logical_sector + media_ptr->fx_media_hidden_sectors, media_ptr->fx_media_driver_sectors);
			media_ptr->fx_media_driver_status =  FX_SUCCESS;
			break;
		}
		
		case FX_DRIVER_FLUSH:
		{
			media_ptr->fx_media_driver_status =  FX_SUCCESS;
			break;
		}

		case FX_DRIVER_ABORT:
		{
			media_ptr->fx_media_driver_status =  FX_SUCCESS;
			break;
		}
		
		case FX_DRIVER_BOOT_READ:
		{
			SD_ReadDisk((uint8_t*)media_ptr->fx_media_driver_buffer, 0, 1);	//读取MBR+DPT
			_fx_partition_offset_calculate(media_ptr -> fx_media_driver_buffer, 0, &boot_start, &boot_size);		//从0扇区数据中获得boot区地址和大小
			SD_ReadDisk((uint8_t*)media_ptr->fx_media_driver_buffer, boot_start, 1);	//读取boot区
			media_ptr->fx_media_driver_status =  FX_SUCCESS;
			break;
		}
		
		case FX_DRIVER_BOOT_WRITE:
		{
			SD_WriteDisk((uint8_t*)media_ptr->fx_media_driver_buffer, boot_start, 1);
			media_ptr->fx_media_driver_status =  FX_SUCCESS;
			break;
		}
		
		default:
		{
			media_ptr->fx_media_driver_status =  FX_IO_ERROR;
			break;
		}
	}
}