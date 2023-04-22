#include "stm32f10x.h"

unsigned char data_byte;



/***********室内************/
#define DHT11_GPIO        GPIOB//定义DHT11_GPIO的引脚
#define DHT11_PIN         GPIO_Pin_14 //定义DHT11_PIN的引脚是
#define DHT11_RCC         RCC_APB2Periph_GPIOB //定义DHT_RCC的时钟
#define DHT11_IO_1        GPIO_SetBits(DHT11_GPIO,DHT11_PIN)   //设置DHT11的引脚输出高电平  


#define DHT11_IO_0        GPIO_ResetBits(DHT11_GPIO,DHT11_PIN)  //设置DHT11的引脚输出低电平  
#define DHT11_ReadBit()   GPIO_ReadInputDataBit(DHT11_GPIO,DHT11_PIN)  //设置MCU读取DHT11的引脚电平  


void delay_ms(u16 nms);
void delay_us(u32 nus);

void DHT1DelayNms(int ms)
{       
	delay_ms(ms);
}

/***延时10us***/
void DHT1DelayNms10us()
{
	delay_us(10);
}
/***延时30us***/
void DHT11Delay30us()		//@11.0592MHz
{
	delay_us(30);
}

int mm=2000;
/********主机发出开始信号*******/
unsigned char DHT11_Start()           //请结合时序图分析
{
		unsigned char i;
         DHT11_IO_1;
         DHT1DelayNms10us();
         DHT11_IO_0;
         DHT1DelayNms(20);   //保持电平时间大于18ms
         DHT11_IO_1;
				for(i = 0;i<250;i++)
				{
					if( DHT11_ReadBit() == 0 )
					{
					 //判断从机是否发出 80us 的低电平响应信号是否结束
					 mm=2000;
					 while( DHT11_ReadBit() == 0 && mm--);
					//判断从机是否发出 80us 的高电平，如发出则进入数据接收状态
					 mm=2000;
					 while(DHT11_ReadBit() == 1&& mm--);
					 return 1;
					}
				}
					 return 0;				
				
}
/********DHT11接收一个字节数据*******/
unsigned char receive_data()
{
         unsigned char i;
         data_byte=0;
         for(i=0;i<8;i++)
         {
                data_byte<<=1;  
					 mm=2000;
                while((!DHT11_ReadBit()&& mm--));//等待50us低电平结束
								DHT11Delay30us();
                if(DHT11_ReadBit()==1)    
                {
									data_byte|=0x1;
									 					 mm=2000;
                   while((DHT11_ReadBit()&& mm--));
                }      
								else
								{
												 
								} 
         }
         return data_byte;
}
/********DHT11 receive data*******/
void DHT11_receive( int *h, int *t)
{
	unsigned char temp;

		unsigned char Tem_Z,Tem_X,Hum_Z,Hum_X,Check;

		if(DHT11_Start())
		{
			DHT11_IO_1;      //Pulled wait DHT11 response

			//Determine whether to issue a slave 80us high level, if you begin collecting data issued                
			Hum_Z = receive_data();   //采集温度整数部分
			Hum_X = receive_data();   //采集温度小数部分
			Tem_Z = receive_data();   //采集湿度整数部分
			Tem_X = receive_data();   //采集湿度小数部分
			Check = receive_data();   //Collection parity bit
			DHT11_IO_1;
			//校验
				temp = Hum_Z;
				temp +=Hum_X;
				temp +=Tem_Z;	
				temp +=Tem_X;			
				//校验通过
				if( temp == Check )
				{
					*h = Hum_Z;
					*t = Tem_Z;			
				}			
		
		}
}

