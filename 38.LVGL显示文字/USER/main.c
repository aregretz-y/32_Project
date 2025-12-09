#include "main.h"

LV_FONT_DECLARE(my_font);

BaseType_t xReturned;										//记录函数返回值
TaskHandle_t Init_Task_Handle = NULL;		//记录初始化任务句柄
TaskHandle_t Lvgl_Task_Handle = NULL;		//记录LVGL任务句柄

u16 xdip;
u16 ydip;

lv_obj_t *lable1;
lv_obj_t *lable2;

void lv_example_label_3(void)
{
    lv_obj_t * cz_label = lv_label_create(lv_scr_act());
    lv_label_set_text(cz_label,"一二三");
    lv_obj_set_style_text_font(cz_label, &my_font, 0);
    lv_obj_set_width(cz_label, 310);
    lv_obj_align(cz_label, LV_ALIGN_BOTTOM_LEFT, 5, -5);
}


//LVGL任务的入口
void Lvgl_Task(void *arg)
{
	for(;;)
	{
		//LVGL通过该函数进行组件切换等
		lv_timer_handler();
	  
		vTaskDelay(5); //阻塞延时5ms，让出CPU
	}

}

//初始化任务的入口
void Init_Task(void *arg)
{
	//硬件初始化
	USART1_Init(115200);
	
	//LVGL的初始化
	lv_init();
	
	//底层设备驱动初始化
	lv_port_disp_init();
	lv_port_indev_init();
	
	//创建自定义的UI界面
	lv_example_label_3();

	//创建LVGL任务
  xReturned = xTaskCreate(
														Lvgl_Task,       	//函数指针，指向任务入口函数 void xxx(void *arg)
														"task for lvgl",  //任务名称，用于调试目的
														512,      				//堆栈大小，以字为单位的，1字 = 4字节
														NULL,    					//任务参数，一般不需要传参，填NULL
														1,							  //任务优先级，数字越大，则优先级越高
														&Lvgl_Task_Handle //任务句柄，如果不需要句柄，可以填NULL
													);     
	
	vTaskDelete( NULL ); //删除任务本身

}


//tick的钩子函数，需要用户自行定义，该函数会周期性的调用 调用频率是1000HZ
void vApplicationTickHook( void )
{
	lv_tick_inc(1);//节拍累计，必须周期性的调用
}



//程序的入口
int main(void)
{

	//1.创建一个初始化任务  Init_Task_Handle
	xReturned = xTaskCreate(
														Init_Task,       	//函数指针，指向任务入口函数 void xxx(void *arg)
														"task for init",  //任务名称，用于调试目的
														512,      				//堆栈大小，以字为单位的，1字 = 4字节
														NULL,    					//任务参数，一般不需要传参，填NULL
														1,							  //任务优先级，数字越大，则优先级越高
														&Init_Task_Handle //任务句柄，如果不需要句柄，可以填NULL
													);     
	//错误处理
	if( xReturned == pdPASS )
	{
			
	}
	
	//2.启动调度器
	vTaskStartScheduler();
	
	while(1) 
	{		
		
	} 
	
}




