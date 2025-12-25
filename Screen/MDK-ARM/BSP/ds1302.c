#include "include.h"

volatile TIMEData Time_now;
volatile TIMEData Time_DS1302;
TIMEData Time_start = {2025,7,03,12,0,0,7};//å¼€å§‹æ—¶é—´		
TIMEData Time_haidilao;							
TIMEData event_power[4];
int YearTemp;
int MonthTemp;
int DayTemp;
int HourTemp;
int MinuteTemp;


uint8_t read_time[7];
char date[9];
char time[6];

void ds1032_DATAOUT_init() //ÅäÖÃË«ÏòI/O¶Ë¿ÚÎªÊä³öÌ¬
{
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	__HAL_RCC_GPIOB_CLK_ENABLE();
	/*Configure GPIO pin Output Level */
//	HAL_GPIO_WritePin(GPIOB, DS1302_DATA_Pin, GPIO_PIN_RESET);

	GPIO_InitStruct.Pin = DS1302_DATA_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void ds1032_DATAINPUT_init() //ÅäÖÃË«ÏòI/O¶Ë¿ÚÎªÊäÈëÌ¬
{
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	__HAL_RCC_GPIOB_CLK_ENABLE();
	GPIO_InitStruct.Pin = DS1302_DATA_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(DS1302_DATA_GPIO_Port, &GPIO_InitStruct);
}

void ds1302_write_onebyte(uint8_t data) //ÏòDS1302·¢ËÍÒ»×Ö½ÚÊı¾İ
{
	ds1032_DATAOUT_init();
	uint8_t count = 0;
	SCLK_L;
	for (count = 0; count < 8; count++) {
		SCLK_L;
		if (data & 0x01) 
		{
			DATA_H;
		} else {
			DATA_L;
		} //ÏÈ×¼±¸ºÃÊı¾İÔÙ·¢ËÍ
		SCLK_H; //À­¸ßÊ±ÖÓÏß£¬·¢ËÍÊı¾İ
		data >>= 1;
	}
} 

void ds1302_wirte_rig(uint8_t address, uint8_t data) //ÏòÖ¸¶¨¼Ä´æÆ÷µØÖ··¢ËÍÊı¾İ
{
	uint8_t temp1 = address;
	uint8_t temp2 = data;
	CE_L;
	SCLK_L;
	HAL_Delay(1);
	CE_H;
	HAL_Delay(2);
	ds1302_write_onebyte(temp1);
	ds1302_write_onebyte(temp2);
	CE_L;
	SCLK_L;
	HAL_Delay(2);
}

uint8_t ds1302_read_rig(uint8_t address) //´ÓÖ¸¶¨µØÖ·¶ÁÈ¡Ò»×Ö½ÚÊı¾İ
{
	uint8_t temp3 = address;
	uint8_t count = 0;
	uint8_t return_data = 0x00;
	CE_L;
	SCLK_L;
	HAL_Delay(3);
	CE_H;
	HAL_Delay(3);
	ds1302_write_onebyte(temp3);
	ds1032_DATAINPUT_init(); //ÅäÖÃI/O¿ÚÎªÊäÈë
	HAL_Delay(2);
	for (count = 0; count < 8; count++) {
		HAL_Delay(2); //Ê¹µçÆ½³ÖĞøÒ»¶ÎÊ±¼ä
		return_data >>= 1;
		SCLK_H;
		HAL_Delay(4); //Ê¹¸ßµçÆ½³ÖĞøÒ»¶ÎÊ±¼ä
		SCLK_L;
		HAL_Delay(5); //ÑÓÊ±14usºóÔÙÈ¥¶ÁÈ¡µçÑ¹£¬¸ü¼Ó×¼È·
		if (HAL_GPIO_ReadPin(DS1302_DATA_GPIO_Port, DS1302_DATA_Pin)) {
			return_data = return_data | 0x80;
		}

	}
	HAL_Delay(2);
	CE_L;
	DATA_L;
	return return_data;
}

void ds1032_init() {

    // ½âÎöÈÕÆÚ
    int day, month, year;
    parse_date(COMPILE_DATE, &day, &month, &year);

    // ½âÎöÊ±¼ä
    int hour, min, sec;
    parse_time(COMPILE_TIME, &hour, &min, &sec);

    if(Time_now.year==2000||Time_now.minute==0){
    	//¸üĞÂTime_now
    	Time_now.second = sec;
    	Time_now.minute = min;
    	Time_now.hour = hour;
    	Time_now.day = day;
    	Time_now.month = month;
    	Time_now.week = 7;
    	Time_now.year = year;
        // ×ª»»Êı¾İ¸ñÊ½
        uint8_t bcd_sec = int_to_bcd(sec);
        uint8_t bcd_min = int_to_bcd(min);
        uint8_t bcd_hour = int_to_bcd(hour);
        uint8_t bcd_day = int_to_bcd(day);
        uint8_t bcd_month = int_to_bcd(month);
        uint8_t bcd_year = int_to_bcd(year % 100); // È¡Äê·İºóÁ½Î»

        // ³õÊ¼»¯DS1302
        ds1302_wirte_rig(0x8e, 0x00);  // ¹Ø±ÕĞ´±£»¤


        ds1302_wirte_rig(0x80, bcd_sec);    // Ãë
        ds1302_wirte_rig(0x82, bcd_min);    // ·Ö
        ds1302_wirte_rig(0x84, bcd_hour);   // Ê±£¨24Ğ¡Ê±ÖÆ£©
        ds1302_wirte_rig(0x86, bcd_day);    // ÈÕ
        ds1302_wirte_rig(0x88, bcd_month);  // ÔÂ
        ds1302_wirte_rig(0x8a, 0x07);       // ĞÇÆÚ£¨Ğè¶îÍâÌîÈë£¬ÕâÀïÊ¹ÓÃÈÕ£©
        ds1302_wirte_rig(0x8c, bcd_year);   // Äê

        ds1302_wirte_rig(0x8e, 0x80);  // ÆôÓÃĞ´±£»¤
    }

}

void ds1032_update(TIMEData time) 
{
	ds1302_wirte_rig(0x8e, 0x00); //¹Ø±ÕĞ´±£»¤
	ds1302_wirte_rig(0x80, int_to_bcd(time.second)); //secondsÃë
	ds1302_wirte_rig(0x82, int_to_bcd(time.minute)); //minutes·Ö
	ds1302_wirte_rig(0x84, int_to_bcd(time.hour)); //hoursÊ±
	ds1302_wirte_rig(0x86, int_to_bcd(time.day)); //dateÈÕ
	ds1302_wirte_rig(0x88, int_to_bcd(time.month)); //monthsÔÂ
	ds1302_wirte_rig(0x8a, 0x07); //days
	ds1302_wirte_rig(0x8c, int_to_bcd(time.year%100)); //yearÄê
	ds1302_wirte_rig(0x8e, 0x80); //¹Ø±ÕĞ´±£»¤
	HAL_Delay(10);
}

void ds1032_read_time() {
	read_time[0] = ds1302_read_rig(0x81); //¶ÁÃë
	read_time[1] = ds1302_read_rig(0x83); //¶Á·Ö
	read_time[2] = ds1302_read_rig(0x85); //¶ÁÊ±
	read_time[3] = ds1302_read_rig(0x87); //¶ÁÈÕ
	read_time[4] = ds1302_read_rig(0x89); //¶ÁÔÂ
	read_time[5] = ds1302_read_rig(0x8B); //¶ÁĞÇÆÚ
	read_time[6] = ds1302_read_rig(0x8D); //¶ÁÄê
}

void ds1032_read_realTime() {
	ds1032_read_time();  //BCDÂë×ª»»Îª10½øÖÆ
	Time_now.second = (read_time[0] >> 4) * 10 + (read_time[0] & 0x0f);
	Time_now.minute = ((read_time[1] >> 4) & (0x07)) * 10
			+ (read_time[1] & 0x0f);
	Time_now.hour = (read_time[2] >> 4) * 10 + (read_time[2] & 0x0f);
	Time_now.day = (read_time[3] >> 4) * 10 + (read_time[3] & 0x0f);
	Time_now.month = (read_time[4] >> 4) * 10 + (read_time[4] & 0x0f);
	Time_now.week = read_time[5];
	Time_now.year = (read_time[6] >> 4) * 10 + (read_time[6] & 0x0f) + 2000;
}

uint8_t int_to_bcd(int num) {
    // ÌáÈ¡Ê®Î»ºÍ¸öÎ»
	uint8_t tens = num / 10;
	uint8_t units = num % 10;
    // ½«Ê®Î»ºÍ¸öÎ»×ª»»Îª4Î»BCDÂë²¢×éºÏ
    uint8_t bcd = (tens << 4) | units;
    return bcd;
}

// ÔÂ·İ×Ö·û´®×ªÊı×ÖµÄ¸¨Öúº¯Êı
uint8_t month_str_to_num(const char* month) {
    const char* months[] = {"Jan","Feb","Mar","Apr","May","Jun",
                            "Jul","Aug","Sep","Oct","Nov","Dec"};
    for (int i = 0; i < 12; i++) {
        if (strncmp(month, months[i], 3) == 0) {
            return i + 1; // ÔÂ·İ´Ó1¿ªÊ¼
        }
    }
    return 1; // Ä¬ÈÏ1ÔÂ
}

// ´ÓÈÕÆÚ×Ö·û´®ÖĞÌáÈ¡ÈÕ¡¢ÔÂ¡¢Äê
void parse_date(const char* date_str, int* day, int* month, int* year) {
    char month_str[4];
    // ÌáÈ¡ÔÂ·İ
    month_str[0] = date_str[0];
    month_str[1] = date_str[1];
    month_str[2] = date_str[2];
    month_str[3] = '\0';
    *month = month_str_to_num(month_str);

    // ÌáÈ¡ÈÕ
    *day = (date_str[4] - '0') * 10 + (date_str[5] - '0');

    // ÌáÈ¡Äê
    *year = (date_str[7] - '0') * 1000 + (date_str[8] - '0') * 100 +
            (date_str[9] - '0') * 10 + (date_str[10] - '0');
}

// ´ÓÊ±¼ä×Ö·û´®ÖĞÌáÈ¡Ê±¡¢·Ö¡¢Ãë
void parse_time(const char* time_str, int* hour, int* min, int* sec) {
    *hour = (time_str[0] - '0') * 10 + (time_str[1] - '0');
    *min  = (time_str[3] - '0') * 10 + (time_str[4] - '0');
    *sec  = (time_str[6] - '0') * 10 + (time_str[7] - '0');
}

// åˆ¤æ–­æ˜¯å¦æ˜¯é—°å¹´

bool isLeapYear(uint16_t year) {
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
        return true;
    }
    return false;
}

// è®¡ç®—æŸå¹´æŸæœˆçš„å¤©æ•°
uint16_t daysInMonth(uint16_t year, uint16_t month) {
    switch (month) {
        case 1: case 3: case 5: case 7: case 8: case 10: case 12:
            return 31;
        case 4: case 6: case 9: case 11:
            return 30;
        case 2:
            return isLeapYear(year) ? 29 : 28;
        default:
            return 0; // ä¸ä¼šå‘ç”Ÿ
    }
}

// è®¡ç®—ä»åŸºå‡†æ—¥æœŸï¼ˆ2000-01-01ï¼‰åˆ°ç»™å®šæ—¥æœŸçš„å¤©æ•°
uint16_t daysFromBase(uint16_t year, uint16_t month, uint16_t day) {
    uint16_t days = 0;
    for (uint16_t y = 2000; y < year; y++) {
        days += isLeapYear(y) ? 366 : 365;
    }
    for (uint16_t m = 1; m < month; m++) {
        days += daysInMonth(year, m);
    }
    days += day;
    return days;
}


int date_diff(TIMEData time1,TIMEData time2) {
	if(time1.year!=0&&time2.year!=0)
	{
	    int days1 = daysFromBase(time1.year, time1.month, time1.day);
	    int days2 = daysFromBase(time2.year, time2.month, time2.day);
	    int temp = days2 - days1;
	    if (temp>0)
		{
	    	return temp;
		}
	    else {
			return 0;
		}

	}
	else {
		return 0;
	}

}

void update_time()
{
	// è®¾ç½® date å­—ç¬¦ä¸²
	date[0] = (int8_t)(Time_now.year%100 / 10) + '0';
	date[1] = (int8_t)(Time_now.year%100 % 10) + '0';
	date[2] = '-';
	date[3] = (int8_t)(Time_now.month / 10) + '0';
	date[4] = (int8_t)(Time_now.month % 10) + '0';
	date[5] = '-';
	date[6] = (int8_t)(Time_now.day / 10) + '0';
	date[7] = (int8_t)(Time_now.day % 10) + '0';
	date[8] = '\0'; // æ­£ç¡®åœ°åœ¨ date å­—ç¬¦ä¸²æœ«å°¾æ·»åŠ ç»ˆæ­¢ç¬¦

	// è®¾ç½® time å­—ç¬¦ä¸²ï¼Œæ³¨æ„è¿™é‡Œä¸è¦ä¿®æ”¹ date æ•°ç»„
	time[0] = (int8_t)(Time_now.hour / 10) + '0';
	time[1] = (int8_t)(Time_now.hour % 10) + '0';
	time[2] = ':';
	time[3] = (int8_t)(Time_now.minute / 10) + '0';
	time[4] = (int8_t)(Time_now.minute % 10) + '0';
	time[5] = '\0'; // æ­£ç¡®åœ°åœ¨ time å­—ç¬¦ä¸²æœ«å°¾æ·»åŠ ç»ˆæ­¢ç¬¦
}

