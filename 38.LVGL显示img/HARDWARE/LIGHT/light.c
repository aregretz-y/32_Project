#include "light.h"

u16 ADC3_Value[2];//用来存放DMA搬运过来的ADC3转换结果

void ADC3_IN5Init(void)
{
	GPIO_InitTypeDef      GPIO_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef       ADC_InitStructure;
	DMA_InitTypeDef       DMA_InitStructure;

	
	
	//使能时钟  GPIOF  ADC3
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE); 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);  	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);
	
	DMA_InitStructure.DMA_Channel = DMA_Channel_2;  //通道2  选择x:0~7
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(ADC3->DR);//外设起始地址
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)ADC3_Value;//内存起始地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;//DMA搬运方向  外设-->储存器
	DMA_InitStructure.DMA_BufferSize = 2;//搬运的次数  该值每搬运一次 -1   到0则停止DMA事务  除非是循环模式
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设地址递增使能
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//内存地址递增使能
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//外设数据宽度  半字
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//内存空间数据宽度  半字
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;//普通模式  循环模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;//优先级 低 中 高 非常高  优先级一样同时请求看数据流和通道编号谁小就先搬运谁的
	
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;    //缓冲模式   Disable--直接模式   
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;//缓冲阈值选择 1/4 2/4 3/4  4/4---自修
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//突发传输设置  Single--直接传输  4  8  16 
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA2_Stream0,&DMA_InitStructure);
	
	DMA_Cmd(DMA2_Stream0,ENABLE);//使能DMA
	
	//配置GPIO引脚位AN模式---PF7  IN5
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//模拟输入功能
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;//必须浮空输入
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA3---ADC3_IN3
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	

	//ADC共同寄存器配置部分初始化
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//独立ADC模式 各个ADC间独立工作
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;//APB2--84Mhz   /2 = 42Mhz
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;//DMA使能只指针多重ADC模式  不影响独立ADC的DMA使用
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//只针对多重ADC模式下的 转换之间的间隔 独立ADC无用
	ADC_CommonInit(&ADC_CommonInitStructure);
	
	//具体某个单独的ADC配置---ADC3
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;//ADC转换精度
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;//扫描转换模式选择 ENABLE DISABLE  如果你是单通道 就不开扫描 多通道建议开启
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//连续转换模式  连续转换开启与否  看你自己的功能需求
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//禁止外部触发00  01  10  11
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;//选择触发源
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//DR右对齐
	ADC_InitStructure.ADC_NbrOfConversion = 2;//当前ADC待转换通道个数  SQR1---20~23Bit
	ADC_Init(ADC3, &ADC_InitStructure);

	//配置你用到的ADC通道转换顺序----规则配置
	//                                        第几个转换     采样时间      转换总时间=采样时间+12bit转换周期
	ADC_RegularChannelConfig(ADC3, ADC_Channel_5, 1,      ADC_SampleTime_3Cycles);
	ADC_RegularChannelConfig(ADC3, ADC_Channel_3, 2,      ADC_SampleTime_3Cycles);
	
	//ADC  DMA请求设置---ADCx->CR2  DDS位
	ADC_DMARequestAfterLastTransferCmd(ADC3, ENABLE);
	
	//ADCx->CR2  DMA位
	ADC_DMACmd(ADC3,ENABLE);
	
	//使能ADC3---开启供电
	ADC_Cmd(ADC3,ENABLE);
}





//滑动变阻器ADC配置---PA5
void ADC1_IN5Init(void)
{
	GPIO_InitTypeDef      GPIO_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef       ADC_InitStructure;
	
	//使能时钟  GPIOF  ADC3
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);  
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	
	//配置GPIO引脚位AN模式---PF7
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//模拟输入功能
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;//必须浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//ADC共同寄存器配置部分初始化
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//独立ADC模式 各个ADC间独立工作
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;//APB2--84Mhz   /2 = 42Mhz
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;//DMA使能只指针多重ADC模式  不影响独立ADC的DMA使用
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//只针对多重ADC模式下的 转换之间的间隔 独立ADC无用
	ADC_CommonInit(&ADC_CommonInitStructure);
	
	//具体某个单独的ADC配置---ADC1
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;//ADC转换精度
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;//扫描转换模式选择 ENABLE DISABLE  如果你是单通道 就不开扫描 多通道建议开启
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//连续转换模式  连续转换开启与否  看你自己的功能需求
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//禁止外部触发00  01  10  11
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;//选择触发源
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//DR右对齐
	ADC_InitStructure.ADC_NbrOfConversion = 1;//当前ADC待转换通道个数  SQR1---20~23Bit
	ADC_Init(ADC1, &ADC_InitStructure);

	//配置你用到的ADC通道转换顺序----规则配置
	//                                        第几个转换     采样时间      转换总时间=采样时间+12bit转换周期
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 1,      ADC_SampleTime_3Cycles);
	
	//使能ADC3---开启供电
	ADC_Cmd(ADC1,ENABLE);
}












