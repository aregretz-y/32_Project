#include "pot.h"

/**
  * @brief  电位器的配置
  * @param  None
  * @retval None
  */
void Potentiometer_Config(void)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	
	//1.打开ADC时钟，现在使用ADC1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	//2.ADC引脚配置
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); //打开时钟
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;							//选择PA5引脚
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;					//选择模拟模式
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;			//选择无上下拉
  GPIO_Init(GPIOA, &GPIO_InitStructure);								//初始化

	//3.ADC配置和初始化
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;											//单模式
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;										//选择4分频
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;				//不使用DMA直接存储器访问
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;  //采样间隔
  ADC_CommonInit(&ADC_CommonInitStructure);
	
	//4.ADC规则通道配置
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;                     		//使用12bit分辨率
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;																	//禁用扫描模式(单通道)														
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;														//禁用连续转换
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;		//无外部触发
	//ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;												//使用数据右对齐
  ADC_InitStructure.ADC_NbrOfConversion = 1;																		//通道的数量
  ADC_Init(ADC1, &ADC_InitStructure);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 1, ADC_SampleTime_3Cycles);
	
	//5.启动ADC1
	ADC_Cmd(ADC1, ENABLE);
}

/**
  * @brief  PS2摇杆的配置
  * @param  None
  * @retval None
  */
void PS2_Config(void)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	
	//1.打开ADC时钟，现在使用ADC1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); //打开时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE); //打开时钟

	
	//2.ADC引脚配置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_6;//选择PA5引脚
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;					//选择模拟模式
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;			//选择无上下拉
  GPIO_Init(GPIOA, &GPIO_InitStructure);								//初始化
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;							//选择PA5引脚
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;					//选择输入功能检测电平
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;			//选择无上下拉
  GPIO_Init(GPIOE, &GPIO_InitStructure);								//初始化

	//3.ADC配置和初始化
	ADC_CommonInitStructure.ADC_Mode = ADC_DualMode_RegSimult;										//双重模式
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;										//选择4分频
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;				//不使用DMA直接存储器访问
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;  //采样间隔
  ADC_CommonInit(&ADC_CommonInitStructure);
	
	//4.ADC规则通道配置
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;                     		//使用12bit分辨率
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;																	//禁用扫描模式(单通道)														
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;														//禁用连续转换
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;		//无外部触发
	//ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;												//使用数据右对齐
  ADC_InitStructure.ADC_NbrOfConversion = 1;																		//通道的数量
  ADC_Init(ADC1, &ADC_InitStructure);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_3Cycles);
	
	ADC_Init(ADC2, &ADC_InitStructure);
  ADC_RegularChannelConfig(ADC2, ADC_Channel_6, 1, ADC_SampleTime_3Cycles);
	
	//5.启动ADC1
	ADC_Cmd(ADC1, ENABLE);
	ADC_Cmd(ADC2, ENABLE);
}


