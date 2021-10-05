#include "SYSTASK.h"
#include "main.h"
#include "tx_api.h"
#include "IDLE.h"
#include "LEDTask.h"
#include "UART.h"

#define SYSTASK_POOL_SIZE	1024
static uint8_t SYSTaskpool[SYSTASK_POOL_SIZE];
static TX_THREAD SYSTaskHand;

static void SYSTASK(ULONG thread_input)
{
	uint32_t CPUcountOld;
	extern volatile uint32_t CPUcount;
	uint8_t CPUpercent = 0;		//CPU占用率百分比
	uint64_t CPUcountMAX = 0;	//100 tick内，空闲线程运行计数最大值
	
	IDLETaskCreate();					//最先建立空闲线程
	CPUcountOld = CPUcount;								
	tx_thread_sleep(100);			//IDLE TASK运行100 tick，得到最大值
	CPUcountMAX = CPUcount - CPUcountOld;
	
	LEDTaskCreate();					//用户任务
	UART1TaskCreate();
	while(1)
	{
		CPUcountOld = CPUcount;
		tx_thread_sleep(100);
		CPUpercent = (CPUcountMAX - (CPUcount - CPUcountOld))*100/CPUcountMAX;
		
		Printf((uint8_t *)"CPU使用率：0x", sizeof("CPU使用率：0x"));
		PrintfHex(&CPUpercent, 1);
		Printf((uint8_t *)"%\r\n", sizeof("%\r\n"));
	}
}

void SYSTaskCreate()
{
	tx_thread_create(&SYSTaskHand, "SYSTEM Task", SYSTASK, NULL, SYSTaskpool, SYSTASK_POOL_SIZE, 2, 2, TX_NO_TIME_SLICE, TX_AUTO_START);
}