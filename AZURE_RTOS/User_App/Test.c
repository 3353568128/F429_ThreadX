#include "Test.h"
#define SDRAMBASEADDR 0xD0000000

#define TEST_TASK_POOL_SIZE	1024
static uint8_t TestTaskpool[TEST_TASK_POOL_SIZE];
static TX_THREAD TestTaskHand;

uint32_t *wp = NULL;
uint32_t x = 0, ErrCount = 0;
void LTDCTest(ULONG thread_input)
{
	wp = (uint32_t *)SDRAMBASEADDR;
	
	Touch_Init();		//´¥ÃþIO³õÊ¼»¯
	
	for(x=0; x<0x40000; x++)
	{
		wp[x] = 0xff00ffff;
	}
	
	_DMA2D_Fill((void *)(SDRAMBASEADDR+30*480*4+30*4), 480-50, 50, 50, 0xff0000ff, LTDC_PIXEL_FORMAT_ARGB8888);
	
	while(1)
	{
		Touch_Scan();
		tx_thread_sleep(10);
	}
}

void TestTaskCreate()
{
	tx_thread_create(&TestTaskHand, "Test Task", LTDCTest, NULL, TestTaskpool, TEST_TASK_POOL_SIZE, 3, 3, TX_NO_TIME_SLICE, TX_AUTO_START);
}