#include "F429_GX_Drive.h"

uint16_t g_LcdHeight = 272;			/* ��ʾ���ֱ���-�߶� */
uint16_t g_LcdWidth = 480;			/* ��ʾ���ֱ���-��� */

static void stm32f4_24xrgb_buffer_toggle(GX_CANVAS *canvas, GX_RECTANGLE *dirty)
{
	GX_RECTANGLE    Limit;
	GX_RECTANGLE    Copy;
	ULONG           offset;
	INT             copy_width;
	INT             copy_height;

	INT             row;
	INT             src_stride_ulongs;
	INT             dest_stride_ulongs;

	ULONG *get;
	ULONG *put;

	gx_utility_rectangle_define(&Limit, 0, 0, canvas->gx_canvas_x_resolution - 1, canvas->gx_canvas_y_resolution - 1);	//�˷��񽫶���ָ���ľ��Ρ������Ƿֱ���

	if (gx_utility_rectangle_overlap_detect(&Limit, &canvas->gx_canvas_dirty_area, &Copy))		//�˷��񽫼���ṩ�ľ����Ƿ����ص��� ��������ص������񽫷��� GX_TRUE ���ص����Ρ�
	{
		copy_width = Copy.gx_rectangle_right - Copy.gx_rectangle_left + 1;		//�������������ķֱ���
		copy_height = Copy.gx_rectangle_bottom - Copy.gx_rectangle_top + 1;

		/* ��canvas��ȡ������ */
		offset = Copy.gx_rectangle_top * canvas->gx_canvas_x_resolution + Copy.gx_rectangle_left;
		get = canvas ->gx_canvas_memory + offset;		//������������ ���� �ϵ���ʼ�����ַ

		/* ��LCD�Դ��ȡҪ���µ����򣬽�canvas���µ����ݸ��ƽ��� */
		put = (ULONG *) FrameBufer;
		offset = (canvas->gx_canvas_display_offset_y + Copy.gx_rectangle_top)* g_LcdWidth;
		offset += canvas->gx_canvas_display_offset_x + Copy.gx_rectangle_left;
		put += offset;				//���������� ӳ�䵽 �Դ� �ϵ���ʼ��ַ

		src_stride_ulongs = canvas ->gx_canvas_x_resolution;
		dest_stride_ulongs = g_LcdWidth;
		
		for(row = 0; row < copy_height; row++)
		{
			memcpy(put, get, copy_width * 4);
			put += dest_stride_ulongs;
			get += src_stride_ulongs;
		}
	}
}
UINT stm32f4_graphics_driver_setup_24xrgb(GX_DISPLAY *display)
{	
    _gx_display_driver_32argb_setup(display, (VOID*)STM32_SCREEN_HANDLE, stm32f4_24xrgb_buffer_toggle);
    return(GX_SUCCESS);
}


void TouchDrive(void)
{
	static uint8_t s_tp_down = 0;
	static GX_EVENT event;
	
	Touch_Scan();
	event.gx_event_payload.gx_event_pointdata.gx_point_x = touchInfo.x[0];
	event.gx_event_payload.gx_event_pointdata.gx_point_y = touchInfo.y[0];
	event.gx_event_sender = 0;
	event.gx_event_target = 0;
	event.gx_event_display_handle = 0xD0000000;
	
	if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15) == GPIO_PIN_SET)		//����
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
		if (s_tp_down == 0)		//����
		{
			s_tp_down = 1;
			event.gx_event_type = GX_EVENT_PEN_DOWN;
			gx_system_event_send(&event);
		}
		else		//�ƶ�
		{
			event.gx_event_type = GX_EVENT_PEN_DRAG;
			gx_system_event_send(&event);
		}
	}
}