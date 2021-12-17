#include "GX_Task.h"
#define 	  Canvas_Memory    0xD0200000                          /* 设置Canvas地址   */

TX_BYTE_POOL  memory_pool;
uint8_t       *MemoryBlock = (uint8_t *)(0xD0300000); /* 动态内存地址    */
#define 	  GUI_NUMBYTES     1024*1024*8                         /* 设置动态内存大小 */

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


GX_WINDOW 		*pScreen;
GX_WINDOW_ROOT  *root;
void GX_Task(ULONG thread_input)
{	
	Touch_Init();		//触摸IO初始化
	
	tx_byte_pool_create(&memory_pool, "MemoryBlock",  MemoryBlock,  GUI_NUMBYTES);	/* 初始化内存池 */
	gx_system_memory_allocator_set(memory_allocate, memory_free);		/* 注册动态内存申请和释放函数 */
	gx_system_initialize();		/* 初始化GUIX */
	guiapp_display_table[0].canvas_memory = (GX_COLOR *)Canvas_Memory;	/* 设置画布地址 */
	gx_studio_display_configure(0, stm32f4_graphics_driver_setup_24xrgb, LANGUAGE_ENGLISH, DISPLAY_1_THEME_1, &root);
	gx_studio_named_widget_create("window_1", (GX_WIDGET *)root, (GX_WIDGET **)&pScreen);
	gx_studio_named_widget_create("window", (GX_WIDGET *)root, (GX_WIDGET **)&pScreen);
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