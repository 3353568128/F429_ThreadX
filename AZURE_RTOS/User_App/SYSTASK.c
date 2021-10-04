#include "SYSTASK.h"
#include "main.h"
#include "tx_api.h"
#include "IDLE.h"
#include "LEDTask.h"

#define SYSTASK_POOL_SIZE	1024
static uint8_t SYSTaskpool[SYSTASK_POOL_SIZE];
static TX_THREAD SYSTaskHand;
extern uint32_t CPUcount;

uint64_t CPUcountMAX = 0;	//100 tick内，空闲线程运行计数最大值
uint8_t CPUpercent = 0;		//CPU占用率百分比
static void SYSTASK(ULONG thread_input)
{
	IDLETaskCreate();					//最先建立空闲线程
	CPUcount = 0;								
	tx_thread_sleep(100);			//IDLE TASK运行100 tick，得到最大值
	CPUcountMAX = CPUcount;
	
	LEDTaskCreate();					//用户任务
	
	while(1)
	{
		CPUcount = 0;
		tx_thread_sleep(100);
		CPUpercent = CPUcount*100/CPUcountMAX;
	}
}

void SYSTaskCreate()
{
	tx_thread_create(&SYSTaskHand, "SYSTEM Task", SYSTASK, NULL, SYSTaskpool, SYSTASK_POOL_SIZE, 2, 2, TX_NO_TIME_SLICE, TX_AUTO_START);
}