/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_usbx_device.c
  * @author  MCD Application Team
  * @brief   USBX Device applicative file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "app_usbx_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "main.h"
#include "tx_api.h"
#include "ux_api.h"
#include "ux_api.h"
#include "ux_system.h"
#include "ux_utility.h"
#include "ux_device_stack.h"
#include "ux_dcd_stm32.h"
#include "ux_device_descriptors.h"
#include "ux_device_class_storage.h"
#include "ux_device_msc.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
#define USBX_MEMORY_SIZE            (32 * 1024)
#define USBX_APP_BYTE_POOL_SIZE     ((1024 * 10) + (USBX_MEMORY_SIZE))

static UX_SLAVE_CLASS_STORAGE_PARAMETER    storage_parameter;
static uint8_t usbx_pool[USBX_APP_BYTE_POOL_SIZE];

#define USBTASK_POOL_SIZE	4096
static uint8_t USBTaskpool[USBTASK_POOL_SIZE];
static TX_THREAD USBTaskHand;

extern PCD_HandleTypeDef  hpcd_USB_OTG_HS;

static void USBTASK(ULONG thread_input)
{
	static uint32_t uiFirstRun = 0;
	UINT  status =  UX_SUCCESS;
	ULONG device_framework_fs_length;
	ULONG string_framework_length;
	ULONG languge_id_framework_length;
	UCHAR *device_framework_full_speed;
	UCHAR *string_framework;
	UCHAR *language_id_framework;
	
	if(uiFirstRun == 0)
	{
			uiFirstRun = 1;
			
			/* 初始化USBX */
			ux_system_initialize(usbx_pool, USBX_MEMORY_SIZE, UX_NULL, 0);
	}


	/* USB设备协议栈初始化 */    
	device_framework_full_speed = USBD_Get_Device_Framework_Speed(USBD_FS_SPEED, &device_framework_fs_length);
	string_framework = USBD_Get_String_Framework(&string_framework_length);
	language_id_framework = USBD_Get_Language_Id_Framework(&languge_id_framework_length);

	status =  _ux_device_stack_initialize(NULL,
																					0,
																					device_framework_full_speed,
																					device_framework_fs_length,
																					string_framework,
																					string_framework_length,
																					language_id_framework,
																					languge_id_framework_length, UX_NULL);
	if (status != UX_SUCCESS)
	{
			return;
	}		
	
	/* 初始化SD卡 */
//	MX_SDIO_SD_Init();

	/* 1个USB模拟U盘设备 */
	storage_parameter.ux_slave_class_storage_parameter_number_lun = 1;

	/* 设置相关参数并注册读写API */
	storage_parameter.ux_slave_class_storage_parameter_lun[0].ux_slave_class_storage_media_last_lba        = (ULONG)(hsd.SdCard.BlockNbr - 1);
	storage_parameter.ux_slave_class_storage_parameter_lun[0].ux_slave_class_storage_media_block_length    = hsd.SdCard.BlockSize;
	storage_parameter.ux_slave_class_storage_parameter_lun[0].ux_slave_class_storage_media_type            =  0;
	storage_parameter.ux_slave_class_storage_parameter_lun[0].ux_slave_class_storage_media_removable_flag  =  0x80;
	storage_parameter.ux_slave_class_storage_parameter_lun[0].ux_slave_class_storage_media_read            =  app_usb_device_thread_media_read;
	storage_parameter.ux_slave_class_storage_parameter_lun[0].ux_slave_class_storage_media_write           =  app_usb_device_thread_media_write; 
	storage_parameter.ux_slave_class_storage_parameter_lun[0].ux_slave_class_storage_media_status          =  app_usb_device_thread_media_status;

	/* 注册USB模U盘到接口0，配置1*/
	status =  _ux_device_stack_class_register(_ux_system_slave_class_storage_name, _ux_device_class_storage_entry, 1, 0, (VOID *)&storage_parameter);
																																								
	if (status != UX_SUCCESS)
	{
			return;
	}

	/* 设置TX FIFO和RX FIFO */
	HAL_PCDEx_SetRxFiFo(&hpcd_USB_OTG_HS, 128);
	HAL_PCDEx_SetTxFiFo(&hpcd_USB_OTG_HS, 0, 64);
	HAL_PCDEx_SetTxFiFo(&hpcd_USB_OTG_HS, 1, 128);

	/* 注册STM32到USBX协议栈并初始化 */
	status =  ux_dcd_stm32_initialize((ULONG)USB_OTG_HS, (ULONG)&hpcd_USB_OTG_HS);		
	if (status != UX_SUCCESS)
	{
			return;
	}
	
	HAL_PCD_Start(&hpcd_USB_OTG_HS);
				
	while(1)
	{
		tx_thread_sleep(30);
	}
}

void USBTaskCreate()
{
	tx_thread_create(&USBTaskHand, "USB Task", USBTASK, NULL, USBTaskpool, USBTASK_POOL_SIZE, 3, 3, TX_NO_TIME_SLICE, TX_AUTO_START);
}
/* USER CODE END PFP */
/**
  * @brief  Application USBX Device Initialization.
  * @param memory_ptr: memory pointer
  * @retval int
  */
UINT MX_USBX_Device_Init(VOID *memory_ptr)
{
  UINT ret = UX_SUCCESS;
  TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL*)memory_ptr;

  /* USER CODE BEGIN MX_USBX_Device_MEM_POOL */
  (void)byte_pool;
  /* USER CODE END MX_USBX_Device_MEM_POOL */

  /* USER CODE BEGIN MX_USBX_Device_Init */

  /* USER CODE END MX_USBX_Device_Init */

  return ret;
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
