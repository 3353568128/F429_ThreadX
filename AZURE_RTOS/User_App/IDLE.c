#include "IDLE.h"
#include "main.h"
#include "tx_api.h"

#define IDLETASK_POOL_SIZE	512
static uint8_t IDLETaskpool[IDLETASK_POOL_SIZE];
static TX_THREAD IDLETaskHand;

volatile uint32_t CPUcount=0;
static void IDLETASK(ULONG thread_input)
{
	while(1)
	{
		CPUcount++;
	}
}

void IDLETaskCreate()
{
	tx_thread_create(&IDLETaskHand, "IDLE Task", IDLETASK, NULL, IDLETaskpool, IDLETASK_POOL_SIZE, 31, 31, TX_NO_TIME_SLICE, TX_AUTO_START);
}