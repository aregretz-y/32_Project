 //  x,y在液晶屏上的显示位置，我的液晶屏是320*240 ，竖屏显示
#include <stdio.h>
#include "stm32f4xx.h"  //用户必须包含
          //  color 是画笔的颜色，BkColor 是背景颜色

          u8 fnShowTxt( u16 x, u16 y , u16 color ,u16 BkColor )
          {
              u8 res=0;                      //  SD卡函数的返回值
      u8 buff[100]={0};          //   存储从txt文档中读到的100个字节的内码
      u8 bitbuff[32]={0};         //   存储从点阵字库获得的32个字节的点阵数据
      u8 NeiMaH,NeiMaL;     //   GB2312内码的高位和低位
  
      u8 CntnuF=1;               //   用来 判断是不是读到文档的末尾了，如果读到字节的个数小于100则表示读到末尾了

      u16 offset=0;                //   读txt文档的偏移地址
      u8 i=0;  

      uint16_t Hznum,bytenum;
// 实际读回的内码字节个数、点阵字节个数
      

              u16 x0,y0;                     // 点阵显示的位置

      u8 charCnt=0;               // 读取的100个字节内码中有几个字符，如果字符个数是偶数则下次偏移再偏移100，如果是奇数，

                                                   // 则读取的100个字节中的最后一个字节可能是下一个汉字的内码高位字节，则偏移99，下次再把这个字节读上。

   x0=x;
   y0=y;

  f_mount(&fs1,"",1);            // 挂载SD卡
  res=f_open(&f1,ReadPath,FA_OPEN_EXISTING|FA_READ);    // 打开字库文件
  if(res!=0)
          {
       fnShowString(10,120,"open hzk fail",16,RED,WHITE,0);
printf("res=%x",res);
return 1;
          }

res=f_open(&ftxt,TxtPath,FA_OPEN_EXISTING|FA_READ);        // 打开txt文档
if(res!=0)
        {
     fnShowString(10,120,"open txt fail",16,RED,WHITE,0);
printf("res=%x",res);
return 1;
        }

f_lseek(&ftxt,offset);                                          // 初始化偏移为0从头开始读。
res=f_read(&ftxt,buff,100,&Hznum);                 // 每次读100个内码字节
while(CntnuF)                                                   //  循环判断是不是读到文件末尾了
{ 
for(i=0;i<Hznum;i++)                                 //  判断读到的字节是汉字还是字符
{ 
if(x>220)                                            // 显示位置的判断
{
 x=x0;
y+=16;
} 
if((y>300))  
{
fnRefreshscreen(WHITE);
        x=x0;
y=y0;
}

        if(buff[i]>0x80)                                     // 是不是汉字 
{
        f_lseek(&f1,32*((buff[i]-0xa0-1)*94+(buff[i+1]-0xa0-1)));
   // 点阵字库内的偏移
res=f_read(&f1,bitbuff,32,&bytenum);
fnShowHzk(x,y,bitbuff,color,BkColor);

x+=16;
                                 i++;                                                             // 这个i++非常重要，因为一个汉字两个字节，除了判断语句i++,

                                                                                                     // 这里需要还要一个 
                                memset(bitbuff,0,sizeof(bitbuff));

} 
else                                                      // 可能是标点也可能是换行符 
{
if(buff[i]==0x0D)                           // 换行标志
y+=16;
else
                                     fnShowChar( x,y,buff[i],16,color,BkColor,0);        // 字符
                                x+=8; 
        charCnt++;                                   // 字符个数计算，用于判断下一次读txt文档的偏移地址
} 
}

       // 计算txt文档的偏移地址

if(Hznum!=100)   //   判断是不是读到文档末尾了
{
 CntnuF=0;
}
else                           // 没有读到末尾继续读 
{
    if(charCnt%2==0)    // 字符个数是偶数，100个字节内码里边正好成对出现，地址偏移+100
offset +=100; 
    else
offset +=99;
memset(buff,0,sizeof(buff));


     f_lseek(&ftxt,offset);                                  //  txt 文档地址偏移
             res=f_read(&ftxt,buff,100,&Hznum);         //  读内码数据
}
}

f_close(&f1);                                //  关闭打开的点阵字库
f_close(&ftxt);                               // 关闭打开的txt文档
f_mount(&fs1,"",NULL);              //  卸载磁盘

         return 0; 
    }
