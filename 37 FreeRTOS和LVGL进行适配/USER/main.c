#include "main.h"


BaseType_t xReturned;										//记录函数返回值
TaskHandle_t Init_Task_Handle = NULL;		//记录初始化任务句柄
TaskHandle_t Lvgl_Task_Handle = NULL;		//记录LVGL任务句柄


u16 xdip;
u16 ydip;

lv_obj_t *lable1;
lv_obj_t *lable2;

//按钮的回调函数
void button_callback(lv_event_t *event)
{
	char buff1[10];
	char buff2[10];
	
	sprintf(buff1,"x:%d",xdip);
	sprintf(buff2,"y:%d",ydip);
	
	switch(event->code)
	{
		case LV_EVENT_PRESSED:
			lv_label_set_text(lable1,buff1);
			lv_label_set_text(lable2,buff2);
		break;

	}
}

//自定义的UI界面函数
void lv_example_btn_1(void)
{
	//在当前活跃的屏幕上创建一个标签组件
	lable1 = lv_label_create(lv_scr_act());
	lv_obj_align(lable1,LV_ALIGN_TOP_LEFT,0,0);

	lable2 = lv_label_create(lv_scr_act());
	lv_obj_align_to(lable2,lable1,LV_ALIGN_OUT_BOTTOM_MID,0,0);

	lv_obj_t * btn1 = lv_btn_create(lv_scr_act());
	lv_obj_set_size(btn1,100,100);
	lv_obj_add_event_cb(btn1,button_callback,LV_EVENT_ALL,NULL);
	lv_obj_align(btn1, LV_ALIGN_CENTER, 0, 0);
  
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
	lv_example_btn_1();

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




