#include "w25q128.h"


/**
  * @brief  串行Flash芯片的初始化
  * @param  None
  * @retval None
  */
void W25Q128_Config(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	//打开时钟
	RCC_AHB1PeriphClockCmd(W25Q128_SCK_RCC|W25Q128_MOSI_RCC|W25Q128_MISO_RCC|W25Q128_CS_RCC, ENABLE);
	
	//配置SCK
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Pin 	= W25Q128_SCK_PIN;
  GPIO_Init(W25Q128_SCK_PORT, &GPIO_InitStructure);
	
	//配置MOSI
	GPIO_InitStructure.GPIO_Pin 	= W25Q128_MOSI_PIN;
  GPIO_Init(W25Q128_MOSI_PORT, &GPIO_InitStructure);
	
	//配置CS
	GPIO_InitStructure.GPIO_Pin 	= W25Q128_CS_PIN;
  GPIO_Init(W25Q128_CS_PORT, &GPIO_InitStructure);
	
	//配置MISO
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Pin 	= W25Q128_MISO_PIN;
  GPIO_Init(W25Q128_MISO_PORT, &GPIO_InitStructure);
	
	//默认处于空闲状态
	W25Q128_CS(1);
}

#if 1
/**
	* @brief  W25Q128发送字节
  * @param  None
	* @retval 返回的是从机发送的内容
  * @note   采用的SPI接口,是同步通信,打算采用模式0,MSB格式
  */
uint8_t W25Q128_SendByte(uint8_t byte)
{
	uint8_t data = 0; //记录收到的内容
	uint8_t i = 0;    
	
	//1.确保SCK引脚为空闲状态
	W25Q128_SCK(0);
	delay_us(5);
	
	//2.循环发送8bit
	for(i=0;i<8;i++)
	{
		//3.准备待发送的bit 
		if( byte & 0x80 )
			W25Q128_MOSI(1);
		else
			W25Q128_MOSI(0);
		
		byte <<= 1;
		
		//4.拉高SCK，此时第1个边沿出现
		W25Q128_SCK(1);
	  delay_us(5);
		
		//5.接收从机发送的bit
		data <<= 1;
		data |= W25Q128_MISO_READ();
		
		//6.拉低SCK，此时第2个边沿出现
		W25Q128_SCK(0);
	  delay_us(5);
	}
	
	return data;
}

#endif


/**
  * @brief  W25Q128读取ID
  * @param  None
  * @retval 返回厂商ID和设备ID  0xEF17
  */
uint16_t W25Q128_ReadID(void)
{
	uint8_t mid = 0,did = 0;
	uint16_t id = 0;
	
	//1.拉低片选，表示通信开始
	W25Q128_CS(0);
	
	//2.发送指令
	W25Q128_SendByte(0x90);
	
	//3.发送地址 MSB
	W25Q128_SendByte(0x00);
	W25Q128_SendByte(0x00);
	W25Q128_SendByte(0x00);
	
	//4.接收字节
	mid = W25Q128_SendByte(0xFF);
	
	//5.接收字节
	did = W25Q128_SendByte(0xFF);
	
	//6.拉高片选，通信结束
	W25Q128_CS(1);
	
	//7.合并ID
	id = (mid<<8) | did;
	
	return id;
}



/**
  * @brief  W25Q128写使能
  * @param  None
  * @retval None
  */
void W25Q128_WriteEnable(void)
{
	
	//1.拉低片选，表示通信开始
	W25Q128_CS(0);
	
	//2.发送指令
	W25Q128_SendByte(0x06);
	
	//3.拉高片选，通信结束
	W25Q128_CS(1);

}




/**
  * @brief  W25Q128写失能
  * @param  None
  * @retval None
  */
void W25Q128_WriteDisable(void)
{
	
	//1.拉低片选，表示通信开始
	W25Q128_CS(0);
	
	//2.发送指令
	W25Q128_SendByte(0x04);
	
	//3.拉高片选，通信结束
	W25Q128_CS(1);

}


/**
  * @brief  W25Q128读取状态寄存器
  * @param  None
  * @retval None
  * @note   BUSY位置位表示忙碌，复位表示空闲
  */
void W25Q128_WaitForWriteEnd(void)
{
	uint8_t status = 0;
	
	//1.拉低片选，表示通信开始
	W25Q128_CS(0);
	
	//2.发送指令
	W25Q128_SendByte(0x05);
	
	//3.读取状态并等待状态成立
	do
  {
    
    status = W25Q128_SendByte(0xFF);

  }
  while ((status & 0x01) == SET);
	
	//4.拉高片选，通信结束
	W25Q128_CS(1);
	
}


/**
  * @brief  W25Q128扇区擦除
  * @param  SectorAddr:待擦除的扇区地址
  * @retval None
  */
void W25Q128_SectorEarse(uint32_t SectorAddr)  
{

	//1.写使能
	W25Q128_WriteEnable();
	
	//2.拉低片选，表示通信开始
	W25Q128_CS(0);
	
	//3.发送指令
	W25Q128_SendByte(0x20);
	
	//4.发送地址 MSB
	W25Q128_SendByte((SectorAddr & 0xFF0000) >> 16 );  //H  
	W25Q128_SendByte((SectorAddr & 0xFF00) >> 8  );    //M 
	W25Q128_SendByte(SectorAddr  & 0xFF);  						 //L  
	
	//5.拉高片选，通信结束
	W25Q128_CS(1);
	
	//6.等待擦除完成
	W25Q128_WaitForWriteEnd();
	
	//7.写失能
	W25Q128_WriteDisable();
	
	printf("Sector Earse OK\r\n");
}

/**
  * @brief  W25Q128读取数据
  * @param  pBuffer: 接收缓冲区
  * @param  ReadAddr: 指的是待读取的地址
  * @param  NumByteToRead: 待读取的字节数量
  * @retval None
  */
void W25Q128_ReadBuffer(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
  //1.拉低片选，表示通信开始
	W25Q128_CS(0);
	
	//2.发送指令
	W25Q128_SendByte(0x03);
	
	//3.发送地址 MSB
	W25Q128_SendByte((ReadAddr & 0xFF0000) >> 16 );  //H  
	W25Q128_SendByte((ReadAddr & 0xFF00) >> 8  );    //M 
	W25Q128_SendByte(ReadAddr  & 0xFF);  						 //L  
	
	//4.循环读取
  while (NumByteToRead--) /*!< while there is data to be read */
  {
    /*!< Read a byte from the FLASH */
    *pBuffer = W25Q128_SendByte(0xFF);
    /*!< Point to the next location where the byte read will be saved */
    pBuffer++;
  }

  //5.拉高片选，通信结束
	W25Q128_CS(1);
}


/**
  * @brief  W25Q128页编程
  * @note   待写入的字节数量不能超过一页大小(256byte)
  * @param  pBuffer:数据缓冲区，存储待写入的数据
  * @param  WriteAddr: 指的是待写入的Flash地址
  * @param  NumByteToWrite: 待写入的字节数量，必须等于或者小于一页大小
  * @retval None
  */
void W25Q128_WritePage(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  //1.写使能
	W25Q128_WriteEnable();
	
	//2.拉低片选，表示通信开始
	W25Q128_CS(0);
	
	//3.发送指令
	W25Q128_SendByte(0x02);
	
	//4.发送地址 MSB
	W25Q128_SendByte((WriteAddr & 0xFF0000) >> 16 );  //H  
	W25Q128_SendByte((WriteAddr & 0xFF00) >> 8  );    //M 
	W25Q128_SendByte(WriteAddr  & 0xFF);  						//L  

  //5.循环写入数据
  while (NumByteToWrite--)
  {
    /*!< Send the current byte */
    W25Q128_SendByte(*pBuffer);
    /*!< Point on the next byte to be written */
    pBuffer++;
  }

  //6.拉高片选，通信结束
	W25Q128_CS(1);
	
	//7.等待擦除完成
	W25Q128_WaitForWriteEnd();
	
	//8.写失能
	W25Q128_WriteDisable();
}

/**
  * @brief  W25Q128写入一个缓存区数据，可以超过一页
  * @note   待写入的字节数量不能超过一页大小(256byte)
  * @param  pBuffer:数据缓冲区，存储待写入的数据
  * @param  WriteAddr: 指的是待写入的Flash地址
  * @param  NumByteToWrite: 待写入的字节数量
  * @retval None
  */
//void W25Q128_WriteBuffer(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
//{
//	uint8_t addr;		//起始地址
//	uint8_t count;	//起始页剩余多少
//	uint8_t numofpage;	//需要写多少页
//	uint8_t numofsingle;	//最后一页剩余多少

//	addr  =  WriteAddr % NUMOFPAGE;
//	count =  NUMOFPAGE - addr;
//	numofpage 	= NumByteToWrite / NUMOFPAGE;
//	numofsingle = NumByteToWrite % NUMOFPAGE;
//	
//	if(addr == 0)//若从一页的首地址开始
//	{
//		if(numofpage == 0)//若一页能写完全部
//		{
//			W25Q128_WritePage(pBuffer,WriteAddr,NumByteToWrite);
//		}
//		else		//若一页写不完
//		{
//			while(numofpage--)
//			{
//				W25Q128_WritePage(pBuffer,WriteAddr,NUMOFPAGE);
//				WriteAddr += NUMOFPAGE;
//				pBuffer 	+= NUMOFPAGE;
//			}
//			W25Q128_WritePage(pBuffer,WriteAddr,numofsingle);
//		}
//	}
//	else //若不是从一页的首地址开始写
//	{
//		if(NumByteToWrite > count)//第一页写不完
//		{
//			W25Q128_WritePage(pBuffer,WriteAddr,count);//写满第一页
//			WriteAddr += count;
//			pBuffer += count;
//			NumByteToWrite -= count;
//			numofpage 	= NumByteToWrite / NUMOFPAGE;
//			numofsingle = NumByteToWrite % NUMOFPAGE;
//			if(numofpage == 0)//若一页能写完全部
//			{
//				W25Q128_WritePage(pBuffer,WriteAddr,NumByteToWrite);
//			}
//			else		//若一页写不完
//			{
//				while(numofpage--)
//				{
//					W25Q128_WritePage(pBuffer,WriteAddr,NUMOFPAGE);
//					WriteAddr += NUMOFPAGE;
//					pBuffer 	+= NUMOFPAGE;
//				}
//				W25Q128_WritePage(pBuffer,WriteAddr,numofsingle);
//			}
//		}
//		else //第一页能写完
//		{
//			W25Q128_WritePage(pBuffer,WriteAddr,NumByteToWrite);
//		}
//	}
//}
void W25Q128_WriteBuffer(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

  Addr = WriteAddr % sFLASH_SPI_PAGESIZE;
  count = sFLASH_SPI_PAGESIZE - Addr;
  NumOfPage =  NumByteToWrite / sFLASH_SPI_PAGESIZE;
  NumOfSingle = NumByteToWrite % sFLASH_SPI_PAGESIZE;

  if (Addr == 0) /*!< WriteAddr is sFLASH_PAGESIZE aligned  */
  {
    if (NumOfPage == 0) /*!< NumByteToWrite < sFLASH_PAGESIZE */
    {
      W25Q128_WritePage(pBuffer, WriteAddr, NumByteToWrite);
    }
    else /*!< NumByteToWrite > sFLASH_PAGESIZE */
    {
      while (NumOfPage--)
      {
        W25Q128_WritePage(pBuffer, WriteAddr, sFLASH_SPI_PAGESIZE);
        WriteAddr +=  sFLASH_SPI_PAGESIZE;
        pBuffer += sFLASH_SPI_PAGESIZE;
      }

      W25Q128_WritePage(pBuffer, WriteAddr, NumOfSingle);
    }
  }
  else /*!< WriteAddr is not sFLASH_PAGESIZE aligned  */
  {
    if (NumOfPage == 0) /*!< NumByteToWrite < sFLASH_PAGESIZE */
    {
      if (NumOfSingle > count) /*!< (NumByteToWrite + WriteAddr) > sFLASH_PAGESIZE */
      {
        temp = NumOfSingle - count;

        W25Q128_WritePage(pBuffer, WriteAddr, count);
        WriteAddr +=  count;
        pBuffer += count;

        W25Q128_WritePage(pBuffer, WriteAddr, temp);
      }
      else
      {
        W25Q128_WritePage(pBuffer, WriteAddr, NumByteToWrite);
      }
    }
    else /*!< NumByteToWrite > sFLASH_PAGESIZE */
    {
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / sFLASH_SPI_PAGESIZE;
      NumOfSingle = NumByteToWrite % sFLASH_SPI_PAGESIZE;

      W25Q128_WritePage(pBuffer, WriteAddr, count);
      WriteAddr +=  count;
      pBuffer += count;

      while (NumOfPage--)
      {
        W25Q128_WritePage(pBuffer, WriteAddr, sFLASH_SPI_PAGESIZE);
        WriteAddr +=  sFLASH_SPI_PAGESIZE;
        pBuffer += sFLASH_SPI_PAGESIZE;
      }

      if (NumOfSingle != 0)
      {
        W25Q128_WritePage(pBuffer, WriteAddr, NumOfSingle);
      }
    }
  }
}


