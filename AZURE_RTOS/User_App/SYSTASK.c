#include "SYSTASK.h"
#include "main.h"
#include "tx_api.h"
#include "IDLE.h"
#include "LEDTask.h"

#define SYSTASK_POOL_SIZE	1024
static uint8_t SYSTaskpool[SYSTASK_POOL_SIZE];
static TX_THREAD SYSTaskHand;
extern uint32_t CPUcount;

uint64_t CPUcountMAX = 0;	//100 tick�ڣ������߳����м������ֵ
uint8_t CPUpercent = 0;		//CPUռ���ʰٷֱ�
static void SYSTASK(ULONG thread_input)
{
	IDLETaskCreate();					//���Ƚ��������߳�
	CPUcount = 0;								
	tx_thread_sleep(100);			//IDLE TASK����100 tick���õ����ֵ
	CPUcountMAX = CPUcount;
	
	LEDTaskCreate();					//�û�����
	
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