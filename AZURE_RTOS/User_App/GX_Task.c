#include "GX_Task.h"
#define 	  Canvas_Memory    0xD0200000                          /* ����Canvas��ַ   */

TX_BYTE_POOL  memory_pool;
uint8_t       *MemoryBlock = (uint8_t *)(0xD0300000); /* ��̬�ڴ��ַ    */
#define 	  GUI_NUMBYTES     1024*1024*8                         /* ���ö�̬�ڴ��С */

extern GX_STUDIO_DISPLAY_INFO guiapp_display_table[1];
VOID *memory_allocate(ULONG size)
{
    VOID *memptr;

    if (tx_byte_allocate(&memory_pool, &memptr, size, TX_NO_WAIT) == TX_SUCCESS)
    {
        return memptr;
    }
    return NULL;
}

void memory_free(VOID *mem)
{
    tx_byte_release(mem);
}

void slide_animation_start(GX_WINDOW *window);
GX_ANIMATION 		slide_animation;
GX_WINDOW 		*pScreen,*pScreen1,*pScreen2;
GX_WINDOW_ROOT  *root;
void GX_Task(ULONG thread_input)
{	
	Touch_Init();		//����IO��ʼ��
	
	tx_byte_pool_create(&memory_pool, "MemoryBlock",  MemoryBlock,  GUI_NUMBYTES);	/* ��ʼ���ڴ�� */
	gx_system_memory_allocator_set(memory_allocate, memory_free);		/* ע�ᶯ̬�ڴ�������ͷź��� */
	gx_system_initialize();		/* ��ʼ��GUIX */
	guiapp_display_table[0].canvas_memory = (GX_COLOR *)Canvas_Memory;	/* ���û�����ַ */
	gx_studio_display_configure(0, stm32f4_graphics_driver_setup_24xrgb, LANGUAGE_ENGLISH, DISPLAY_1_THEME_1, &root);
	gx_studio_named_widget_create("window", (GX_WIDGET *)root, (GX_WIDGET **)&pScreen);
	gx_studio_named_widget_create("window_1", (GX_WIDGET *)GX_NULL, (GX_WIDGET **)&pScreen1);
	gx_studio_named_widget_create("window_2", (GX_WIDGET *)GX_NULL, (GX_WIDGET **)&pScreen2);
	
	/* pScreen2 ���ӵ� pScreen������Ϊ���Ӵ��� */
	gx_widget_attach((GX_WIDGET *)pScreen, (GX_WIDGET *)pScreen2);
	
	/* �����������ܲ����� */
	gx_animation_create(&slide_animation);
	slide_animation_start((GX_WINDOW *)pScreen);
	
	gx_widget_show(root);
	gx_system_start();
	while(1)
	{
		TouchDrive();
		tx_thread_sleep(1);
	}
}


#define GX_TASK_POOL_SIZE	1024
static uint8_t GX_Taskpool[GX_TASK_POOL_SIZE];
static TX_THREAD GX_TaskHand;

void GX_TaskCreate()
{
	tx_thread_create(&GX_TaskHand, "GuiX Task", GX_Task, NULL, GX_Taskpool, GX_TASK_POOL_SIZE, 3, 3, TX_NO_TIME_SLICE, TX_AUTO_START);
}

//#include "stdio.h"
//#define TEMPLEN 1
//#define GUI_ID_Timer0   10
//INT chart_data[400] = {0};
//INT chart_data_buff[400] = {0};
//INT chart_data_temp[10] = {0};
//UINT _cbEventWindow0(GX_WINDOW *widget, GX_EVENT *event_ptr)
//{
//	int i;
//	static uint32_t count = 0;
//	char buf[20] = {0};
//	volatile int status = 0;

//	if((chart_data_buff[0]==0)&&(chart_data_buff[1]==0))
//	{
//		for(i = 0; i<400; i++)
//		{
//			chart_data_buff[i] = rand()%100;
//		}
//	}
//	
//	switch (event_ptr->gx_event_type)
//	{
//		/* �ؼ���ʾ�¼� */
//		case GX_EVENT_SHOW:
//			/* ����һ�� GUIX ��ʱ�� */
//			gx_system_timer_start((GX_WIDGET *)widget, GUI_ID_Timer0, 1, 4);
//			/* Ĭ���¼����� */
//			gx_window_event_process(widget, event_ptr);
//		break;
//		
//		/* ��ʱ��ʱ������¼�*/
//		case GX_EVENT_TIMER:
//			if (event_ptr->gx_event_payload.gx_event_timer_id == GUI_ID_Timer0)
//			{
//				sprintf(buf, "%d", count++);
//				gx_prompt_text_set((GX_PROMPT *)&(window.window_prompt_2), buf);  
//				for(i = 0; i<TEMPLEN; i++)
//				{
//					chart_data_temp[i] = rand()%100;
//				}
//				memcpy(chart_data, &chart_data_buff[TEMPLEN], (400-TEMPLEN)*sizeof(int));
//				memcpy(&chart_data[400-TEMPLEN], chart_data_temp, TEMPLEN*sizeof(int));
//				memcpy(chart_data_buff, chart_data, 400*sizeof(int));
//				/* ���� Line Chart �ؼ� */
//				gx_line_chart_update(&(window.window_line_chart), chart_data, 400);
//			}
//		break;
//			
//		case GX_SIGNAL(button01, GX_EVENT_CLICKED):
//			status = 0;
//		break;
//		
//		case GX_SIGNAL(checkbox00, GX_EVENT_TOGGLE_ON):
//			buf[0] = 0;
//		break;
//		
////		case GX_SIGNAL(checkbox00, GX_EVENT_TOGGLE_OFF):
////			status = 0;
////		break;
//			
//		default:
//		return gx_window_event_process(widget, event_ptr);
//	}
//	return 0;
//}


GX_WIDGET *win_list[] = { (GX_WIDGET *)&window_2, (GX_WIDGET *)&window_1, GX_NULL };

void slide_animation_start(GX_WINDOW *window)
{
	GX_ANIMATION_INFO slide_animation_info = {0};
	slide_animation_info.gx_animation_parent = (GX_WIDGET *)window;
	slide_animation_info.gx_animation_style = GX_ANIMATION_SCREEN_DRAG | GX_ANIMATION_HORIZONTAL | GX_ANIMATION_WRAP | GX_ANIMATION_CUBIC_EASE_OUT;
	slide_animation_info.gx_animation_id = GX_NEXT_ANIMATION_ID;
	slide_animation_info.gx_animation_frame_interval = 2;
	slide_animation_info.gx_animation_steps = 10;
	slide_animation_info.gx_animation_slide_screen_list = win_list;
	gx_animation_drag_enable(&slide_animation, (GX_WIDGET *)window, &slide_animation_info);
}


UINT slide_win_event_process(GX_WINDOW *window, GX_EVENT *event_ptr)
{
 switch (event_ptr->gx_event_type)
 {
	 case GX_EVENT_SHOW:
		slide_animation_start(window);
	 return gx_widget_event_process(window, event_ptr);
	 
	 case GX_EVENT_HIDE:
		gx_animation_drag_disable(&slide_animation, (GX_WIDGET *)window);
	 return gx_widget_event_process(window, event_ptr);
	 
	 default:
	 return gx_window_event_process(window, event_ptr);
 } 
}