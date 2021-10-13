/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ux_device_msc.c
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
#include "ux_device_msc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* USER CODE BEGIN 1 */
UINT  app_usb_device_thread_media_read(VOID *storage, ULONG lun, UCHAR *data_pointer, ULONG number_blocks, ULONG lba, ULONG *media_status)
{
	HAL_SD_ReadBlocks(&hsd, data_pointer, lba, number_blocks,10);
	while(HAL_SD_GetCardState(&hsd) != HAL_SD_CARD_TRANSFER);
	return 0;
}

UINT  app_usb_device_thread_media_write(VOID *storage, ULONG lun, UCHAR *data_pointer, ULONG number_blocks, ULONG lba, ULONG *media_status)
{
	HAL_SD_WriteBlocks(&hsd, data_pointer, lba, number_blocks,10);
	while(HAL_SD_GetCardState(&hsd) != HAL_SD_CARD_TRANSFER);
	return 0;
}

UINT  app_usb_device_thread_media_status(VOID *storage, ULONG lun, ULONG media_id, ULONG *media_status)
{
    return(UX_SUCCESS);
}
/* USER CODE END 1 */
