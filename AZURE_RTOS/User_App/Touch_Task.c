#include "Touch_Task.h"
#include "main.h"
#include "tx_api.h"
#include "touch_480x272.h"

#define TouchTASK_POOL_SIZE	1024
static uint8_t TouchTaskpool[TouchTASK_POOL_SIZE];
static TX_THREAD TouchTaskHand;

static void TouchTASK(ULONG thread_input)
{
	static uint8_t s_tp_down = 0;
	GX_EVENT event;
	
	Touch_Init();
	
	while(1)
	{
		Touch_Scan();
		event.gx_event_payload.gx_event_pointdata.gx_point_x = touchInfo.x[0];
		event.gx_event_payload.gx_event_pointdata.gx_point_y = touchInfo.y[0];
		event.gx_event_sender = 0;
		event.gx_event_target = 0;
		event.gx_event_display_handle = 0xD0100000;
		
		if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15) == GPIO_PIN_SET)		//松手
		{
			if (s_tp_down == 1)
			{
				s_tp_down = 0;
				event.gx_event_type = GX_EVENT_PEN_UP;
				gx_system_event_send(&event);
			}
		}
		else
		{
			if (s_tp_down == 0)		//按下
			{
				s_tp_down = 1;
				event.gx_event_type = GX_EVENT_PEN_DOWN;
				gx_system_event_send(&event);
			}
			else		//移动
			{
				event.gx_event_type = GX_EVENT_PEN_DRAG;
				gx_system_event_send(&event);
			}
		}
		tx_thread_sleep(10);
	}
}

void TouchTaskCreate()
{
	tx_thread_create(&TouchTaskHand, "LED Task", TouchTASK, NULL, TouchTaskpool, TouchTASK_POOL_SIZE, 3, 3, TX_NO_TIME_SLICE, TX_AUTO_START);
}