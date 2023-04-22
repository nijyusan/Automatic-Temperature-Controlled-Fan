#include "stm32f10x.h"

unsigned char data_byte;



/***********����************/
#define DHT11_GPIO        GPIOB//����DHT11_GPIO������
#define DHT11_PIN         GPIO_Pin_14 //����DHT11_PIN��������
#define DHT11_RCC         RCC_APB2Periph_GPIOB //����DHT_RCC��ʱ��
#define DHT11_IO_1        GPIO_SetBits(DHT11_GPIO,DHT11_PIN)   //����DHT11����������ߵ�ƽ  


#define DHT11_IO_0        GPIO_ResetBits(DHT11_GPIO,DHT11_PIN)  //����DHT11����������͵�ƽ  
#define DHT11_ReadBit()   GPIO_ReadInputDataBit(DHT11_GPIO,DHT11_PIN)  //����MCU��ȡDHT11�����ŵ�ƽ  


void delay_ms(u16 nms);
void delay_us(u32 nus);

void DHT1DelayNms(int ms)
{       
	delay_ms(ms);
}

/***��ʱ10us***/
void DHT1DelayNms10us()
{
	delay_us(10);
}
/***��ʱ30us***/
void DHT11Delay30us()		//@11.0592MHz
{
	delay_us(30);
}

int mm=2000;
/********����������ʼ�ź�*******/
unsigned char DHT11_Start()           //����ʱ��ͼ����
{
		unsigned char i;
         DHT11_IO_1;
         DHT1DelayNms10us();
         DHT11_IO_0;
         DHT1DelayNms(20);   //���ֵ�ƽʱ�����18ms
         DHT11_IO_1;
				for(i = 0;i<250;i++)
				{
					if( DHT11_ReadBit() == 0 )
					{
					 //�жϴӻ��Ƿ񷢳� 80us �ĵ͵�ƽ��Ӧ�ź��Ƿ����
					 mm=2000;
					 while( DHT11_ReadBit() == 0 && mm--);
					//�жϴӻ��Ƿ񷢳� 80us �ĸߵ�ƽ���緢����������ݽ���״̬
					 mm=2000;
					 while(DHT11_ReadBit() == 1&& mm--);
					 return 1;
					}
				}
					 return 0;				
				
}
/********DHT11����һ���ֽ�����*******/
unsigned char receive_data()
{
         unsigned char i;
         data_byte=0;
         for(i=0;i<8;i++)
         {
                data_byte<<=1;  
					 mm=2000;
                while((!DHT11_ReadBit()&& mm--));//�ȴ�50us�͵�ƽ����
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
			Hum_Z = receive_data();   //�ɼ��¶���������
			Hum_X = receive_data();   //�ɼ��¶�С������
			Tem_Z = receive_data();   //�ɼ�ʪ����������
			Tem_X = receive_data();   //�ɼ�ʪ��С������
			Check = receive_data();   //Collection parity bit
			DHT11_IO_1;
			//У��
				temp = Hum_Z;
				temp +=Hum_X;
				temp +=Tem_Z;	
				temp +=Tem_X;			
				//У��ͨ��
				if( temp == Check )
				{
					*h = Hum_Z;
					*t = Tem_Z;			
				}			
		
		}
}

