#include "LEDTask.h"
#include "main.h"
#include "tx_api.h"

#define LEDTASK_POOL_SIZE	1024
static uint8_t LEDTaskpool[LEDTASK_POOL_SIZE];
static TX_THREAD LEDTaskHand;
static void LEDTASK(ULONG thread_input)
{
	while(1)
	{
		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7,0);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12,1);
		tx_thread_sleep(30);
		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_7,1);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12,0);
		tx_thread_sleep(30);
	}
}

void LEDTaskCreate()
{
	tx_thread_create(&LEDTaskHand, "LED Task", LEDTASK, NULL, LEDTaskpool, LEDTASK_POOL_SIZE, 3, 3, TX_NO_TIME_SLICE, TX_AUTO_START);
}