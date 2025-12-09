#include "oled.h"
#include "stdlib.h"
#include "oledfont.h"//字库头文件 英文字库--ASCII  	 
#include "delay.h"
#include "iic.h"
#include "spi.h"

u8 OLED_GRAM[144][8];//自定义的用户显存


//反显函数 0--黑底白字  1---白底黑字
void OLED_ColorTurn(u8 i)
{
	if(i==0)
	{
		OLED_WR_Byte(0xA6,OLED_CMD);//正常显示
	}
	if(i==1)
	{
		OLED_WR_Byte(0xA7,OLED_CMD);//反色显示
	}
}

//屏幕旋转180度
void OLED_DisplayTurn(u8 i)
{
	if(i==0)
	{
		OLED_WR_Byte(0xC8,OLED_CMD);//反转显示  COM63~COM0
		OLED_WR_Byte(0xA1,OLED_CMD);//          SEG127~SEG0
	}
	if(i==1)
	{
		OLED_WR_Byte(0xC0,OLED_CMD);//正常显示  COM0~COM63
		OLED_WR_Byte(0xA0,OLED_CMD);//          SEG0~SEG127
	}
}

//发送一个字节
//向SSD1306写入一个字节。
//mode:数据/命令标志 0,表示命令;1,表示数据;
int OLED_WR_Byte(u8 Data,u8 DC)
{
	u8 i;
	//先通过控制D/C拉高或拉低选择是发命令还是数据（IIC是通过发送0x00/0x40控制字节）
	if(DC)
	  OLED_DC = 1;//D/C= 1 发图像数据
	else
	  OLED_DC = 0;//D/C = 0 发指令
	
	
	SPI_CS = 0;//CS = 0
	SPI0_ReadWriteByte(Data);//使用SPI发完了8bit
	SPI_CS = 1;//CS = 1 结束通信
	
	
	OLED_DC = 1;//最后回到默认的OLED的数据模式   
	
	
	
//	IIC_Start();
//    IIC_WriteByte(0x78);//发送写模式的设备地址
//    if(1 == IIC_ReadAck())
//    {
//         IIC_Stop();
//         return -1;//无应答就直接结束 返回函数   
//    }
//    
//    //发送控制字节
//    if(DC == 0)
//        IIC_WriteByte(0x00);//发指令
//    else
//        IIC_WriteByte(0x40);//发图像数据
//    if(1 == IIC_ReadAck())
//    {
//         IIC_Stop();
//         return -2;//无应答就直接结束 返回函数   
//    }
//    
//    //发送数据字节
//    IIC_WriteByte(Data);
//    if(1 == IIC_ReadAck())
//    {
//         IIC_Stop();
//         return -3;//无应答就直接结束 返回函数   
//    }
//    
//    //成功发送完成 结束
//    IIC_Stop();
//    return 0;
}

//开启OLED显示 
void OLED_DisPlay_On(void)
{
	OLED_WR_Byte(0x8D,OLED_CMD);//电荷泵使能
	OLED_WR_Byte(0x14,OLED_CMD);//开启电荷泵
	OLED_WR_Byte(0xAF,OLED_CMD);//点亮屏幕
}

//关闭OLED显示 
void OLED_DisPlay_Off(void)
{
	OLED_WR_Byte(0x8D,OLED_CMD);//电荷泵使能
	OLED_WR_Byte(0x10,OLED_CMD);//关闭电荷泵
	OLED_WR_Byte(0xAE,OLED_CMD);//关闭屏幕
}



//更新显存到OLED	
void OLED_Refresh(void)
{
	u8 i,n;
	for(i=0;i<8;i++)
	{
	   OLED_WR_Byte(0xb0+i,OLED_CMD); //设置页起始地址--PAGE0~PAGE7
	   
									  //  0000 0000 ~ 0111 1111
									 //设置列地址 分为了高三位和低四位 分开设置
		                             //0000 0000
									 //118 0111 0110
	   OLED_WR_Byte(0x00,OLED_CMD);   //设置低列起始地址 0~127
	   OLED_WR_Byte(0x10,OLED_CMD);   //设置高列起始地址
		
	   for(n=0;n<128;n++)  //OLED_GRAM[128][8]
		 OLED_WR_Byte(OLED_GRAM[n][i],OLED_DATA);
  }
}

//x：0~127列
//page : 0~7 页
//这个函数是老师写的按照列行式取模的显示字符代码---对应的字库取模方式一定得是列行式
//如果不想自己取字库，可以直接使用商家代码即可
void OLED_DrawChar(int x,int page,char ch,int size)
{
	int i;
	//先设置页地址
	OLED_WR_Byte(0xB0+page,OLED_CMD);
	
	//设置列地址 0~127  000 0000~ 111 1111
	OLED_WR_Byte(x & 0x0F,OLED_CMD);//提取出列地址低四位
	OLED_WR_Byte(((x>>4)&0x07) | 0x10,OLED_CMD);//提取出列地址的高四位内低三位
	
	//先判断哪种字体
	switch(size)
	{
		case 6:
			//显示6*8数据
//			for(i=0;i<6;i++)
//			{
//				OLED_WR_Byte()
//			}
			break;
		case 8:
			//上半页部分
			for(i=0;i<8;i++)
			{
				OLED_WR_Byte(asc2_1608[ch-32][i],OLED_DATA);
			}
			//下半页部分
			OLED_WR_Byte(0xB0+page+1,OLED_CMD);
			OLED_WR_Byte(x & 0x0F,OLED_CMD);//提取出列地址低四位
			OLED_WR_Byte(((x>>4)&0x07) | 0x10,OLED_CMD);
			
			for(;i<16;i++)
			{
				OLED_WR_Byte(asc2_1608[ch-32][i],OLED_DATA);
			}
			break;
		case 12:
			//上半页部分
			for(i=0;i<12;i++)
			{
				OLED_WR_Byte(asc2_2412[ch-32][i],OLED_DATA);
			}
			//中半页部分
			OLED_WR_Byte(0xB0+page+1,OLED_CMD);
			OLED_WR_Byte(x & 0x0F,OLED_CMD);//提取出列地址低四位
			OLED_WR_Byte(((x>>4)&0x07) | 0x10,OLED_CMD);
			
			for(;i<24;i++)
			{
				OLED_WR_Byte(asc2_2412[ch-32][i],OLED_DATA);
			}
			
			//下半页部分
			OLED_WR_Byte(0xB0+page+2,OLED_CMD);
			OLED_WR_Byte(x & 0x0F,OLED_CMD);//提取出列地址低四位
			OLED_WR_Byte(((x>>4)&0x07) | 0x10,OLED_CMD);
			
			for(;i<36;i++)
			{
				OLED_WR_Byte(asc2_2412[ch-32][i],OLED_DATA);
			}
			break;
	}
	

//	OLED_WR_Byte(0x00,OLED_DATA);
//	OLED_WR_Byte(0x42,OLED_DATA);
//	OLED_WR_Byte(0x7E,OLED_DATA);
//	OLED_WR_Byte(0x40,OLED_DATA);
//	OLED_WR_Byte(0x00,OLED_DATA);
//	OLED_WR_Byte(0x00,OLED_DATA);
}

void OLED_Point(int x,int y)
{
	OLED_GRAM[x][y/8] |= 1<<(y%8);//低位在前
}

void OLED_PointClean(int x,int y)
{
	OLED_GRAM[x][y/8] &= ~(1<<(y%8));
}

//到底商家取模是低位在前还是高位在前?
void OLED_Drawc(int x,int y,char ch,int size)
{
	int i,j;
	int y0 = y;//y0备份下y的初值
	int offset = ch-32;//先算出要显示字符在字库数组中的成员偏移量
	int num = (size*2/8+((size*2%8==0)?0:1))*size;//算出当前字体 对应一个图案取模所占字节总数
	u8 temp;
	//循环num次 每次从对应字体的数组字库中拿一个字节出来
	for(i=0;i<num;i++)
	{
		switch(size)
		{
			//根据不同字体 每次从对应数组取一个字节出来放入temp
			case 6:
				temp = asc2_1206[offset][i];
				break;
			case 8:
				temp = asc2_1608[offset][i];
				break;
			case 12:
				temp = asc2_2412[offset][i];
				break;
		}
		
		//开始对上面取到的一个字节，根据坐标放入OLED_GRAM数组
		//因为一个字节数8bit  所以从高位开始一位位放入数组---OLED_Ponit（）
		for(j=0;j<8;j++)
		{
			if(temp & (0x80>>j))
				OLED_Point(x,y);//画点进去
			else
				OLED_PointClean(x,y);//清除这个点
			y++;//因为我们逐列式，从上往下画，所以y要每次画点后往下走一格
			
			if(y==y0+size*2)//当根据当前字体 显示完一列 要换下一列
			{
				y = y0;
				x++;
				break;//推出8次循环 舍弃剩下的位
			}
		}

	}

	OLED_Refresh();//最后将数组OLED_GRAM[] 全部写到SSD1306 显示出来
}
	



//清屏函数
void OLED_Clear(void)
{
	u8 i,n;
	for(i=0;i<8;i++)
	{
	   for(n=0;n<128;n++)
			{
				OLED_GRAM[n][i]=0;//清除所有数据
			}
  }
	OLED_Refresh();//更新显示
}

//画点 
//x:0~127
//y:0~63
void OLED_DrawPoint(u8 x,u8 y)
{
	u8 i,n;
	i=y/8;//得到是第几页位置 例如坐标y=12  12/8 = 1 在第1号页位置
	//m=y%8;//得到在对应页中的偏移 例如y=12  12%8 = 4 在当前页字节列的第4号像素
	n=1<<(y%8);//n = 0x80>>(y%8) 高位在前  n记录下对位置1的值  0x04  0000 0100
	OLED_GRAM[x][y/8] |=n;//不影响其他像素的前提下填入数组对应位
}

//清除一个点
//x:0~127
//y:0~63
void OLED_ClearPoint(u8 x,u8 y)
{
	u8 i,m,n;
	i=y/8;
	m=y%8;
	n=1<<m;
	OLED_GRAM[x][i]=~OLED_GRAM[x][i];
	OLED_GRAM[x][i]|=n;
	OLED_GRAM[x][i]=~OLED_GRAM[x][i];
}


//画线
//x:0~128
//y:0~64
void OLED_DrawLine(u8 x1,u8 y1,u8 x2,u8 y2)
{
	u8 i,k,k1,k2,y0;
	if((x1<0)||(x2>128)||(y1<0)||(y2>64)||(x1>x2)||(y1>y2))return;
	if(x1==x2)    //画竖线
	{
			for(i=0;i<(y2-y1);i++)
			{
				OLED_DrawPoint(x1,y1+i);
			}
  }
	else if(y1==y2)   //画横线
	{
			for(i=0;i<(x2-x1);i++)
			{
				OLED_DrawPoint(x1+i,y1);
			}
  }
	else      //画斜线
	{
		k1=y2-y1;
		k2=x2-x1;
		k=k1*10/k2;
		for(i=0;i<(x2-x1);i++)
			{
			  OLED_DrawPoint(x1+i,y1+i*k/10);
			}
	}
}
//x,y:圆心坐标
//r:圆的半径
void OLED_DrawCircle(u8 x,u8 y,u8 r)
{
	int a, b,num;
    a = 0;
    b = r;
    while(2 * b * b >= r * r)      
    {
        OLED_DrawPoint(x + a, y - b);
        OLED_DrawPoint(x - a, y - b);
        OLED_DrawPoint(x - a, y + b);
        OLED_DrawPoint(x + a, y + b);
 
        OLED_DrawPoint(x + b, y + a);
        OLED_DrawPoint(x + b, y - a);
        OLED_DrawPoint(x - b, y - a);
        OLED_DrawPoint(x - b, y + a);
        
        a++;
        num = (a * a + b * b) - r*r;//计算画的点离圆心的距离
        if(num > 0)
        {
            b--;
            a--;
        }
    }
}



//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//size:选择字体 12/16/24
//取模方式 逐列式
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size1)
{
	u8 i,m,temp,size2,chr1;
	u8 y0=y;
	size2=(size1/8+((size1%8)?1:0))*(size1/2);  //得到字体一个字符对应点阵集所占的字节数
	chr1=chr-' ';  //计算偏移后的值
	for(i=0;i<size2;i++)
	{
		if(size1==12)
        {
			temp=asc2_1206[chr1][i];
		} //调用1206字体
		else if(size1==16)
        {
			temp=asc2_1608[chr1][i];
		} //调用1608字体
		else if(size1==24)
        {
			temp=asc2_2412[chr1][i];
		} //调用2412字体
		else 
			return;
		
		for(m=0;m<8;m++)           //写入数据
		{
			if(temp&0x80)
				OLED_DrawPoint(x,y);//画点
			else 
				OLED_ClearPoint(x,y);//清除点
			temp<<=1;//高位先画
			y++;
			if((y-y0)==size1)
			{
				y=y0;
				x++;
				break;//当前字节不管有没有画完。都离开
			}
		}
	}
}


//显示字符串
//x,y:起点坐标  
//size1:字体大小 
//*chr:字符串起始地址 
void OLED_ShowString(u8 x,u8 y,u8 *chr,u8 size1)
{
	while((*chr>=' ')&&(*chr<='~'))//判断是不是非法字符!
	{
		OLED_ShowChar(x,y,*chr,size1);
		x+=size1/2;
		if(x>128-size1)  //换行
		{
			x=0;
			y+=2;
    }
		chr++;
  }
}

//m^n
u32 OLED_Pow(u8 m,u8 n)
{
	u32 result=1;
	while(n--)
	{
	  result*=m;
	}
	return result;
}

////显示2个数字
////x,y :起点坐标	 
////len :数字的位数
////size:字体大小
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size1)
{
	u8 t,temp;
	for(t=0;t<len;t++)
	{
		temp=(num/OLED_Pow(10,len-t-1))%10;
			if(temp==0)
			{
				OLED_ShowChar(x+(size1/2)*t,y,'0',size1);
      }
			else 
			{
			  OLED_ShowChar(x+(size1/2)*t,y,temp+'0',size1);
			}
  }
}

//显示汉字
//x,y:起点坐标
//num:汉字对应的序号
//取模方式 列行式
void OLED_ShowChinese(u8 x,u8 y,u8 num,u8 size1)
{
	u8 i,m,n=0,temp,chr1;
	u8 x0=x,y0=y;
	u8 size3=size1/8;//算出一个中文字 高度显示上占几个页
	
	//每次循环都把一页搞定
	while(size3--)
	{
		chr1=num*size1/8+n;//算出当前要显示中字的在数组中的第一维偏移量
		n++;
		
		for(i=0;i<size1;i++)
		{
			if(size1==16)
					{temp=Hzk1[chr1][i];}//调用16*16字体
			else if(size1==24)
					{temp=Hzk2[chr1][i];}//调用24*24字体
			else if(size1==32)       
					{temp=Hzk3[chr1][i];}//调用32*32字体
			else if(size1==64)
					{temp=Hzk4[chr1][i];}//调用64*64字体
			else return;
						
			for(m=0;m<8;m++)
			{
				if(temp&0x01)
					OLED_DrawPoint(x,y);
				else 
					OLED_ClearPoint(x,y);
				temp>>=1;
				y++;//y当前页 固定只加8次
			}
			x++;//换列
			if((x-x0)==size1)//设定x叠加上限 size1
			{
				//能进来zheli表示你已经把上半页显示完成
				x=x0;//列还原
				y0=y0+8;//
			}
			y=y0;
		 }
	}
}

//num 显示汉字的个数
//space 每一遍显示的间隔
void OLED_ScrollDisplay(u8 num,u8 space)
{
	u8 i,n,t=0,m=0,r;
	while(1)
	{
		if(m==0)
		{
	    OLED_ShowChinese(128,24,t,16); //写入一个汉字保存在OLED_GRAM[][]数组中
			t++;
		}
		if(t==num)
			{
				for(r=0;r<16*space;r++)      //显示间隔
				 {
					for(i=0;i<144;i++)
						{
							for(n=0;n<8;n++)
							{
								OLED_GRAM[i-1][n]=OLED_GRAM[i][n];
							}
						}
           OLED_Refresh();
				 }
        t=0;
      }
		m++;
		if(m==16){m=0;}
		for(i=0;i<144;i++)   //实现左移
		{
			for(n=0;n<8;n++)
			{
				OLED_GRAM[i-1][n]=OLED_GRAM[i][n];
			}
		}
		OLED_Refresh();
	}
}

//配置写入数据的起始位置
void OLED_WR_BP(u8 x,u8 y)
{
	OLED_WR_Byte(0xb0+y,OLED_CMD);//设置行起始地址
	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
	OLED_WR_Byte((x&0x0f),OLED_CMD);
}

//x0,y0：起点坐标
//x1,y1：终点坐标
//BMP[]：要写入的图片数组
//图片取模一定是 列行式
void OLED_ShowPicture(u8 x0,u8 y0,u8 x1,u8 y1,u8 BMP[])
{
	u32 j=0;
	u8 x=0,y=0;
	if(y%8==0)y=0;
	else y+=1;
	for(y=y0;y<y1;y++)
	 {
		 OLED_WR_BP(x0,y);
		 for(x=x0;x<x1;x++)
		 {
			 OLED_WR_Byte(BMP[j],OLED_DATA);
			 j++;
     }
	 }
}



//OLED的初始化
void OLED_Init(void)
{
	//IIC_GPIOInit();//初始化IIC引脚 PB8---SCK
	
	SPI_GPIOInit();//初始化SPI 引脚
	
	//SPi对OLED屏芯片复位
	OLED_RES = 0;
	delay_ms(200);
	OLED_RES = 1;
	
	//发送各种SSD1306指令先配置好OLED显示参数
	OLED_WR_Byte(0xAE,OLED_CMD);//--turn off oled panel
	OLED_WR_Byte(0x00,OLED_CMD);//---set low column address
	OLED_WR_Byte(0x10,OLED_CMD);//---set high column address
	
	OLED_WR_Byte(0x40,OLED_CMD);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	OLED_WR_Byte(0x81,OLED_CMD);//--set contrast control register
	OLED_WR_Byte(0xCF,OLED_CMD);// Set SEG Output Current Brightness
	
	OLED_WR_Byte(0xA1,OLED_CMD);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
	OLED_WR_Byte(0xC8,OLED_CMD);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
	OLED_WR_Byte(0xA6,OLED_CMD);//--set normal display
	OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
	OLED_WR_Byte(0x3f,OLED_CMD);//--1/64 duty
	OLED_WR_Byte(0xD3,OLED_CMD);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	OLED_WR_Byte(0x00,OLED_CMD);//-not offset
	OLED_WR_Byte(0xd5,OLED_CMD);//--set display clock divide ratio/oscillator frequency
	OLED_WR_Byte(0x80,OLED_CMD);//--set divide ratio, Set Clock as 100 Frames/Sec
	OLED_WR_Byte(0xD9,OLED_CMD);//--set pre-charge period
	OLED_WR_Byte(0xF1,OLED_CMD);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	OLED_WR_Byte(0xDA,OLED_CMD);//--set com pins hardware configuration
	OLED_WR_Byte(0x12,OLED_CMD);
	OLED_WR_Byte(0xDB,OLED_CMD);//--set vcomh
	OLED_WR_Byte(0x40,OLED_CMD);//Set VCOM Deselect Level
	OLED_WR_Byte(0x20,OLED_CMD);//-Set Page Addressing Mode (0x00/0x01/0x02)
	OLED_WR_Byte(0x02,OLED_CMD);//
	OLED_WR_Byte(0x8D,OLED_CMD);//--set Charge Pump enable/disable
	OLED_WR_Byte(0x14,OLED_CMD);//--set(0x10) disable
	OLED_WR_Byte(0xA4,OLED_CMD);// Disable Entire Display On (0xa4/0xa5)
	OLED_WR_Byte(0xA6,OLED_CMD);// Disable Inverse Display On (0xa6/a7) 
	OLED_WR_Byte(0xAF,OLED_CMD);
	OLED_Clear();//清屏
}