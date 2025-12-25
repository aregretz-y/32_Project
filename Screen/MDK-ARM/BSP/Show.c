#include "include.h"

char temp_itoa[30];		

const unsigned char * photo_now = photo1;

void init_show()
{
	EPD_HW_Init(); 													//Electronic paper initialization

//	EPD_ALL_image(gImage1_white,gImage1_xiaowu);	//Refresh the picture in full screen

#ifdef scene1
#ifdef black_white_red
	EPD_WhiteScreen_Black();
#else
	EPD_WhiteScreen_White();
#endif

	EPD_W21_Init();
	//white
	EPD_Dis_Part(0, 0, gImage1_white, 296, 128, NEG);//刷白色背景
	//right
	EPD_Dis_string(186, 0, time, 32, NEG);					//刷时间
	EPD_Dis_string(200, 32, date, 16, NEG);					//刷日期
	EPD_Dis_Part(210, 48, gImage_love, 32, 32, POS);//刷爱心图标
	sprintf(temp_itoa, "%d", date_diff(Time_start, Time_now));//计算开始日结束日
	EPD_Dis_string(210 + 32, 56, temp_itoa, 16, NEG);
	EPD_Dis_Part(210, 80, gImage_haidi, 32, 32, NEG);
	sprintf(temp_itoa, "%d", date_diff(Time_haidilao, Time_now));
	EPD_Dis_string(210 + 32, 88,temp_itoa, 16, NEG);
	//middle
	EPD_Dis_Part(85, 0, photo_now, 110, 128, POS);
	//	left
	EPD_Dis_power(0, 0, (date_diff(event_power[0], Time_now)>4)?0:4-date_diff(event_power[0], Time_now));
	EPD_Dis_power(0, 32, (date_diff(event_power[1], Time_now)>4)?0:4-date_diff(event_power[1], Time_now));
	EPD_Dis_power(0, 64, (date_diff(event_power[2], Time_now)>4)?0:4-date_diff(event_power[2], Time_now));
	EPD_Dis_power(0, 96, (date_diff(event_power[3], Time_now)>4)?0:4-date_diff(event_power[3], Time_now));
	EPD_Dis_Part(60, 0, gImage_riji, 32, 32, NEG);
	EPD_Dis_Part(60, 32, gImage_dushu, 32, 32, POS);
	EPD_Dis_Part(60, 64, gImage_tanzi, 32, 32, POS);
	EPD_Dis_Part(60, 96, gImage_jiaoshui, 32, 32, POS);
#endif

#ifdef scene2
#ifdef black_white_red
	EPD_WhiteScreen_Black();
#else
	EPD_WhiteScreen_White();
#endif
	EPD_W21_Init();
	//white
	EPD_Dis_Part(0, 0, gImage1_white, 296, 128, NEG);
	//	left
	EPD_Dis_Part(0, 0, gImage1_xiaowu, 296, 128, POS);
	//right
	EPD_Dis_string(166, 0, time, 32, NEG);
	EPD_Dis_string(180, 32, date, 16, NEG);
	EPD_Dis_Part(190, 48, gImage_love, 32, 32, POS);
	EPD_Dis_string(190 + 32, 56, itoa(date_diff(Time_start, Time_now),temp_itoa,10), 16, NEG);
	EPD_Dis_Part(190, 80, gImage_huoguo, 32, 32, POS);
	EPD_Dis_string(190 + 32, 88, itoa(date_diff(Time_haidilao, Time_now),temp_itoa,10), 16, NEG);
#endif

#ifdef scene3
#ifdef black_white_red
	EPD_WhiteScreen_Red();
#else
	EPD_WhiteScreen_White();
#endif
	EPD_W21_Init();

	EPD_ALL_image(gImage1_white,gImage1_xiaowu);
#endif
//
	EPD_Part_Update_and_DeepSleep();
}

void update_show()
{
	EPD_W21_Init();
	
	for(int i=0;i<4;i++)
	{
    int diff = date_diff(event_power[i], Time_now);
    sprintf(Text, "DBG: event%d %02d-%02d -> diff=%d grid=%d\r\n",
            i, event_power[i].month, event_power[i].day, diff,
            (diff>4)?0:4-diff);
    HAL_UART_Transmit(&huart1,(uint8_t*)Text,strlen(Text),50);
}

#ifdef scene1
	//white
	EPD_Dis_Part(0, 0, gImage1_white, 296, 128, NEG); 
	//right
	EPD_Dis_string(186, 0, time, 32, NEG);
	EPD_Dis_string(200, 32, date, 16, NEG);
	EPD_Dis_Part(210, 48, gImage_love, 32, 32, POS);
	sprintf(temp_itoa, "%d", date_diff(Time_start, Time_now));
	EPD_Dis_string(210 + 32, 56,temp_itoa, 16, NEG);
	EPD_Dis_Part(210, 80, gImage_haidi, 32, 32, NEG);
	sprintf(temp_itoa, "%d", date_diff(Time_haidilao, Time_now));
	EPD_Dis_string(210 + 32, 88,temp_itoa, 16, NEG);
	//middle
	EPD_Dis_Part(85, 0, photo_now, 110, 128, POS);
	//left
	EPD_Dis_power(0, 0,  (date_diff(event_power[0], Time_now)>4)?0:4-date_diff(event_power[0], Time_now));
	EPD_Dis_power(0, 32, (date_diff(event_power[1], Time_now)>4)?0:4-date_diff(event_power[1], Time_now));
	EPD_Dis_power(0, 64, (date_diff(event_power[2], Time_now)>4)?0:4-date_diff(event_power[2], Time_now));
	EPD_Dis_power(0, 96, (date_diff(event_power[3], Time_now)>4)?0:4-date_diff(event_power[3], Time_now));
	EPD_Dis_Part(60, 0, gImage_riji, 32, 32, NEG);
	EPD_Dis_Part(60, 32, gImage_dushu, 32, 32, POS);
	EPD_Dis_Part(60, 64, gImage_tanzi, 32, 32, POS);
	EPD_Dis_Part(60, 96, gImage_jiaoshui, 32, 32, POS);
#endif

#ifdef scene2
	//white
	EPD_Dis_Part(0, 0, gImage1_white, 296, 128, NEG);
	//	left
	EPD_Dis_Part(0, 0, gImage1_xiaowu, 296, 128, POS);
	//right
	EPD_Dis_string(166, 0, time, 32, NEG);
	EPD_Dis_string(180, 32, date, 16, NEG);
	EPD_Dis_Part(190, 48, gImage_love, 32, 32, POS);
	EPD_Dis_string(190 + 32, 56, itoa(date_diff(Time_start, Time_now),temp_itoa,10), 16, NEG);
	EPD_Dis_Part(190, 80, gImage_huoguo, 32, 32, POS);
	EPD_Dis_string(190 + 32, 88, itoa(date_diff(Time_haidilao, Time_now),temp_itoa,10), 16, NEG);
#endif

#ifdef scene3

#endif
	EPD_Part_Update_and_DeepSleep();
}







