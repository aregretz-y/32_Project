/**
  ******************************************************************************
  * @file    main.c 
  * @author  Your Name
  * @version V0.0.0 (主版本号.次版本号.修订版本号)
  * @date    2025/10/24
  * @brief   这是一个使用定时器去扫描按键，实现检测按键的短按、长按
  ******************************************************************************
  * @attention
	*				
	*			
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"  //用户必须包含

/* Private typedef -----------------------------------------------------------*/

typedef enum
{
    KEY_CHECK = 0,      // 按键检查
    KEY_COMFIRM = 1,    // 按键确认  
    KEY_HOLD = 2,       // 按住不放
    KEY_RELEASE = 3     // 按键释放
} KEY_STATE;

typedef enum 
{
    NULL_KEY = 0,       // 空闲模式
    SHORT_KEY = 1,      // 短按
    LONG_KEY = 2,       // 长按
    HOLD_KEY = 3,       // 按住不放
    KEY_UP = 4          // 按键抬起
} KEY_TYPE;

/* Private define ------------------------------------------------------------*/
//按键对应的IO管脚 KEY1  PA.15
#define KEY_IO_RCC        RCC_APB2Periph_GPIOA      

/* Private macro -------------------------------------------------------------*/
//Key: 1:高电平，按键未按下， 0：低电平，按键按下
#define Key  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)

/* Private variables ---------------------------------------------------------*/


KEY_STATE KeyState = KEY_CHECK;  // 初始化按键状态为检测状态
uint32_t g_KeyFlag = 0;                // 按键有效标志，0： 按键值无效； 1：按键值有效

KEY_TYPE g_KeyActionFlag;		//用于区别长按和短按

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  按键检测函数
  * @param  None
  * @retval None
  */
/**
 * 单个按键检测短按和长按事件
 * 短按：时间 10ms < T < 1 s, 长按：时间 T >1 s
 * 功能：使用状态机方式，扫描单个按键；扫描周期为10ms,10ms刚好跳过抖动；
 * 状态机使用switch case语句实现状态之间的跳转
 * lock变量用于判断是否是第一次进行按键确认状态
 * ：长按键事件提前执行，短按键事件释放后才执行
 */
void Key_Scan(void)
{
    static u8 TimeCnt = 0;
    static u8 lock = 0;
    static u8 hold_reported = 0;  // 按住状态是否已上报
    
    switch (KeyState)
    {
        case KEY_CHECK:    
            if(!Key)   
            {
                KeyState = KEY_COMFIRM;
            }
            TimeCnt = 0;
            lock = 0;
            hold_reported = 0;  // 重置按住状态标志
            break;
            
        case KEY_COMFIRM:
            if(!Key)  // 按键按下
            {
                if(!lock) lock = 1;
               
                TimeCnt++;  
                
                /* 长按判断 */
                if(TimeCnt > 100)  // 1秒
                {
                    g_KeyActionFlag = LONG_KEY;
                    KeyState = KEY_HOLD;  // 进入按住状态
                    TimeCnt = 0;
                }                
            }   
            else  // 按键释放
            {
                if(1==lock)  // 有效短按
                {
                    g_KeyActionFlag = SHORT_KEY;
                    KeyState = KEY_RELEASE;
                }
                else  // 抖动，返回检查状态
                {
                    KeyState = KEY_CHECK;
                }
            } 
            break;
            
        case KEY_HOLD:  // 按住不放状态
            if(!Key)  // 仍然按住
            {
                TimeCnt++;
                
                // 每500ms报告一次按住状态（可选）
                if(TimeCnt >= 50 && !hold_reported)  // 500ms
                {
                    g_KeyActionFlag = HOLD_KEY;
                    hold_reported = 1;  // 标记已上报
                    TimeCnt = 0;
                }
            }
            else  // 按键释放
            {
                g_KeyActionFlag = KEY_UP;  // 报告按键抬起
                KeyState = KEY_RELEASE;
            }
            break;
            
        case KEY_RELEASE:
            if(Key)  // 确认按键已释放
            { 
                KeyState = KEY_CHECK;
            }
            break;
            
        default: 
            KeyState = KEY_CHECK;
            break;
    }    
}

/**
  * @brief  配置TIM3定时2ms
  * @param  None
  * @retval None
  */
void TIM_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;

  //1.打开时钟
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	//2.配置时基---2ms
	TIM_TimeBaseStructure.TIM_Prescaler = 8400 - 1;     // 分频
	TIM_TimeBaseStructure.TIM_Period = 20 - 1;     // 周期：2ms
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	//3.配置NVIC
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;	//抢占优先级
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	TIM_ITConfig(TIM3, TIM_IT_Update,ENABLE); 

	//3.启动定时器
	TIM_Cmd(TIM3, ENABLE);
}

/**
  * @brief  按键配置函数
  * @param  None
  * @retval None
  */
void KEY_Config(void)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;

	//1.打开时钟
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	//2.配置参数，KEY0的，输入模式，并初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure); 
}

/**
  * @brief  LED0 LED1配置函数
  * @param  None
  * @retval None
  */
void LED_Config(void)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;

	//1.打开时钟
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

	
	//2.配置参数，LED0 LED1的，输出模式，并初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_Init(GPIOF, &GPIO_InitStructure); 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_Init(GPIOE, &GPIO_InitStructure); 

}

/**
  * @brief  TIM3的ISR
  * @param  None
  * @retval None
  */
void TIM3_IRQHandler(void)   //TIM3 每 2ms 中断一次
{   
    static u8 cnt;
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
		{
		cnt++;
        if(cnt>5)           //每10ms 执行一次按键扫描程序
        {
            Key_Scan();
            cnt = 0;
        }
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  //清除TIMx的中断待处理位:TIM 中断源 
		}   
}


/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void) 
{     
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	KEY_Config();
	TIM_Config();
	LED_Config();
  /* Infinite loop */
	for(;;)
	{
		switch(g_KeyActionFlag)
        {
            case SHORT_KEY:
						GPIO_ToggleBits(GPIOF, GPIO_Pin_9);
            g_KeyActionFlag = KEY_UP;		//状态回到抬起
            break;
            
            case LONG_KEY:
						GPIO_ToggleBits(GPIOF, GPIO_Pin_10);
            g_KeyActionFlag = NULL_KEY;		//状态回到空
						break;
						
						case HOLD_KEY:
						GPIO_ToggleBits(GPIOE, GPIO_Pin_13);
            g_KeyActionFlag = NULL_KEY;		//状态回到空
						break;
						
						case KEY_UP:
						GPIO_ToggleBits(GPIOE, GPIO_Pin_14);
            g_KeyActionFlag = NULL_KEY;		//状态回到空
						break;
						
            default: break;
				}
	}
}

/************************ (C) COPYRIGHT Your Name *************END OF FILE****/
