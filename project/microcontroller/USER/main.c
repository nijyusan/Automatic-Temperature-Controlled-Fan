#include "sys.h"
#include "delay.h"
#include "stdio.h"
#include "oled_IIC.h"
#include "DHT11.h"
#include "UltrasonicWave.h"


int wd=0;//Temperature
int sd=0;//Humidity
int kaiguan=0;//Switch 1 on  0 off
int fskg=0;//Switch 1 on  0 off
int juli=0;
int dingshi=0 ;  //In minutes
int daojishi=1*60; //Countdown 
int wenkong=0;//0 off   1 on
int wendusd=25;//Temperature setting
int PWM=5;//Fan speed  5   7  11
int dangwei=1;//Gear
void GPIOD_Init(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA |RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC |RCC_APB2Periph_AFIO, ENABLE );	//Enable GPIOA clock
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE); //Disable jtag
	//Fan
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;         //
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
  //Key
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_15;         //
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ;   //output
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;         //
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ;   //
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
	//DHT11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;         //
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD ;   //
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
	//OLED
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13;         //
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
}



//5ms delay
void Delay5Ms(void)
{
	unsigned char i, j;
	i = 54;
	j = 198;
	do
	{
		while (--j);
	} while (--i);
}


//400ms delay
void Delay400Ms(void)
{
	unsigned char i, j, k;

	i = 17;
	j = 208;
	k = 27;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}
unsigned char msg[]={"020 "};	
unsigned char ds[]={"00:00"};	
//Display
void xianshi(unsigned  int x1,unsigned  int x2,unsigned  int x3,unsigned  int x4,unsigned  int x5,unsigned  int x6,unsigned  int x7,unsigned  int x8)
{
	char yyyy=0;
  // 温(0) 度(1) 湿(2) 度(3) 距(4) 离(5) 档(6) 位(7) 温(8) 控(9) 开(10) 关(11) ：(12) 定 (13)  时 (14)
	//温度 Temperature
	OLED_ShowCHinese(0,yyyy,0);//
	OLED_ShowCHinese(16,yyyy,1);//
	msg[0]=x1%1000/100+'0';
	msg[1]=x1%100/10+'0';	
	msg[2]=x1%10/1+'0';	
	OLED_ShowString(32,yyyy,msg,16); 	
	
	//湿度 Humidity
  OLED_ShowCHinese(64,yyyy,2);//
	OLED_ShowCHinese(80,yyyy,3);//
	msg[0]=x2%1000/100+'0';
	msg[1]=x2%100/10+'0';	
	msg[2]=x2%10/1+'0';	
	OLED_ShowString(96,yyyy,msg,16); 
	
	yyyy=2;
	//距离 Distance
	OLED_ShowCHinese(0,yyyy,4);//
	OLED_ShowCHinese(16,yyyy,5);//
	msg[0]=x3%1000/100+'0';
	msg[1]=x3%100/10+'0';	
	msg[2]=x3%10/1+'0';	
	OLED_ShowString(32,yyyy,msg,16); 	
	
	//档位 Gear
  OLED_ShowCHinese(64,yyyy,6);//
	OLED_ShowCHinese(80,yyyy,7);//
	msg[0]=' ';
	msg[1]=x4%10/1+'0';	
	msg[2]=' ';
	OLED_ShowString(96,yyyy,msg,16); 



	yyyy=4;
	//定时 Timing
	OLED_ShowCHinese(0,yyyy,13);//
	OLED_ShowCHinese(16,yyyy,14);//
	ds[0]=x5%100/10+'0';
	ds[1]=x5%10/1+'0';	
	ds[2]=0;	
	OLED_ShowString(32,yyyy,ds,16); 	
	
	//倒计时 Countdown
	ds[0]=x6/60%100/10+'0';
	ds[1]=x6/60%10/1+'0';	
	ds[2]=':';	
	ds[3]=x6%60%100/10+'0';	
	ds[4]=x6%60%10/1+'0';		
	OLED_ShowString(64,yyyy,ds,16); 

  yyyy=6;

	//温控 Temperature control
  OLED_ShowCHinese(24,yyyy,8);//
	OLED_ShowCHinese(40,yyyy,9);//
	if(x7==0)
	{
	 OLED_ShowCHinese(58,yyyy,11);//关	Off	
	}
	else
	{
	 OLED_ShowCHinese(58,yyyy,10);//开	On	
	}
	
	msg[0]=x8%1000/100+'0';
	msg[1]=x8%100/10+'0';	
	msg[2]=x8%10/1+'0';	
	OLED_ShowString(80,yyyy,msg,16); 

 
}
//
void anjiansaomiao(void)
{
	static char k1f=0,k2f=0,k3f=0,k4f=0,k5f=0,k6f=0;
	char k1,k2,k3,k4,k5,k6;
	k1=PAin(11);k2=PAin(12);k3=PAin(15);k4=PBin(3);k5=PBin(4);k6=PBin(5);
	
	

	//Button scanning
	//switch
	if(k1==0)
	{
		k1f=1;
	}
	else
	{
		if(	k1f)
		{
			k1f=0;
			//Switch
      if(kaiguan==0)
			{
				kaiguan=1;
			  daojishi=dingshi*60;//Update countdown
			}				
			else
			{
				kaiguan=0;				
			}
		}
	}	
	
if(kaiguan==1)
{	
	//Gear
	if(k2==0)
	{
		k2f=1;
	}
	else
	{
		if(	k2f)
		{
			k2f=0;	
 			//Gear
      dangwei++;
      if(dangwei>2)
			{
				dangwei=1;
			}				
		}
	}		
	//Timing
	if(k3==0)
	{
		k3f=1;
	}
	else
	{
		if(	k3f)
		{
			k3f=0;	
      dingshi++;
			if(dingshi>10)
			{
				dingshi=0;
			}
			daojishi=dingshi*60;//Update countdown
 			
		}
	}		
  //Temperature control on/off
	if(k4==0)
	{
		k4f=1;
	}
	else
	{
		if(	k4f)
		{
			k4f=0;	
  		//Temperature control on/off
      if(wenkong==0)
			{
				wenkong=1;
			}				
			else
			{
				wenkong=0;				
			}    
 			
		}
	}		
	
  //Temperature +
	if(k5==0)
	{
		k5f=1;
	}
	else
	{
		if(	k5f)
		{
			k5f=0;	
      wendusd++;
 			
		}
	}
  //Temperature -
	if(k6==0)
	{
		k6f=1;
	}
	else
	{
		if(	k6f)
		{
			k6f=0;	
      wendusd--;
 			
		}
	}
}

}
void USART1_init(void)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;	
	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_AFIO   , ENABLE);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);

	//USART1 Tx(PA.09) 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  //USART1 RX2(PA.10) 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
  GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
	
	USART_InitStructure.USART_BaudRate = 9600; 
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; 
	USART_InitStructure.USART_StopBits = USART_StopBits_1; 
	USART_InitStructure.USART_Parity = USART_Parity_No; 
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx; 
 
	//USART1
	USART_Init(USART1, &USART_InitStructure);
	USART_Cmd(USART1, ENABLE); 	
	USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART1, USART_IT_PE, ENABLE);  
	USART_ITConfig(USART1, USART_IT_ERR, ENABLE);		
	
		USART1->DR = (0X55 & (uint16_t)0x01FF);
		while((USART1->SR&0X40)==0);		
		
	
 	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;	  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	

}
void UsartSendData(unsigned char *send_buff,unsigned long int length)
{
 	unsigned long int i = 0;


	for(i = 0;i < length;i ++)
	{	
		USART1->DR = (send_buff[i] & (uint16_t)0x01FF);
		while((USART1->SR&0X40)==0);	
	}	
	
}
int fputc(int ch, FILE* stream)
{
		USART1->DR = (ch & (uint16_t)0x01FF);
		while((USART1->SR&0X40)==0);	
	return ch;
}
//Process the data received  +IPD,0,24:*100*100*100*100*100*100
unsigned int find(unsigned char *buf,unsigned char siz,unsigned char wz,unsigned int x)
{
   
  unsigned char  i=0;
  unsigned char  j=0;
  unsigned char  Z1=0,Z2=0;
  unsigned int  va;
  for(i=0;i<siz;i++)
  {
      if(buf[i]=='*')
	  {
	    j++;
		if(j==wz)
		{
	   	  Z1 = i;		  	 
		}
		else if(j==wz+1)
		{
	   	  Z2 = i;		  	 
		}
	  }

  }
	  //Calculation
	  if((Z2-Z1)==1)
	  {
		 va =  0;
		  return(x);
	  }
	  if((Z2-Z1)==2)
	  {
		 va =  (buf[Z1+1]-'0');
		 	  return(va);
	  }
	  else if((Z2-Z1)==3)
	  {
	     va =  (buf[Z1+1]-'0')*10+(buf[Z1+2]-'0');
		 	  return(va);
	  }
	  else if((Z2-Z1)==4)
	  {
	   va =  (buf[Z1+1]-'0')*100+(buf[Z1+2]-'0')*10+(buf[Z1+3]-'0');
	   	  return(va);
	  }  
	  	  return(x);
}

int RX1_Num=0;//Save the received data
int RX1_JS=0,RX1_KG=0;//Save the received data
int RX1_OK=0;//1 indicates that data was received and 0 indicates that no data was received
unsigned char RX1_BUFF[200];

//Serial port 1 receives interrupted
void USART1_IRQHandler(void)  
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{	
		  USART_ClearITPendingBit(USART1, USART_IT_RXNE);	
		  //Save the received data
		  RX1_BUFF[RX1_Num++]=USART_ReceiveData(USART1);	//Read the received data
		  if(RX1_Num>=200)
			{
				RX1_Num=0;
			}
			 RX1_KG = 1;
			 RX1_JS = 0;

	}
	

	if(USART_GetITStatus(USART1, USART_IT_PE) != RESET)
	{	
		USART_ClearITPendingBit(USART1, USART_IT_PE);
	}		
	if (USART_GetFlagStatus(USART1, USART_IT_LBD) != RESET) 
	{
    USART_ClearITPendingBit(USART1, USART_IT_LBD);           
  }		
	if(USART_GetFlagStatus(USART1, USART_FLAG_ORE) != RESET)
	{
		USART_ReceiveData(USART1);
		USART_ClearFlag(USART1, USART_FLAG_ORE);
	}		
	if(USART_GetFlagStatus(USART1, USART_FLAG_NE) != RESET)
	{
		USART_ClearFlag(USART1, USART_FLAG_NE);
	}
	if(USART_GetFlagStatus(USART1, USART_FLAG_FE) != RESET)
	{
		USART_ClearFlag(USART1, USART_FLAG_FE);
	}
	if(USART_GetFlagStatus(USART1, USART_FLAG_PE) != RESET)
	{
		USART_ClearFlag(USART1, USART_FLAG_PE);
	}	
	if (USART_GetITStatus(USART1, USART_IT_TC) != RESET) 
  {
    USART_ClearITPendingBit(USART1, USART_IT_TC);    
  }	
}
void chuli1(void)
{
	 if(RX1_KG==1)
	 {
		 RX1_JS++;
		 if(RX1_JS>=1)
		 {
			 RX1_KG = 0;
			 RX1_JS = 0;
			 RX1_OK = 1;
			 
	 
		 }
	 }
}
void RX1_CHULI(void)
{
//	long int *p;
	int  shumu=0;
//	int  wz=0;	
	char i=0;
	
	if(RX1_OK==1)
	{
		RX1_OK = 0;
		shumu = RX1_Num;
		RX1_Num = 0;	
		//Start processing the received data
		//Determine the length of the received data			 
		if(RX1_BUFF[0]=='*')
		{
			//Fetch data
			//Lower temperature limit
			wendusd=find(RX1_BUFF,shumu,1,wendusd);
			dingshi=find(RX1_BUFF,shumu,2,dingshi);
      daojishi=dingshi*60;
			
 			dangwei=find(RX1_BUFF,shumu,3,dangwei); 
//      if(dangwei==1)
//			{
//				dangwei=1;
//			}	
//			else if(dangwei==2)
//			{
//				dangwei=2;				
//			}
//			else if(dangwei==3)
//			{
//				dangwei=3;				
//			}
		}		
		if(RX1_BUFF[0]=='#')
		{
      if(RX1_BUFF[1]=='K')//1Temperature control       K on G off
			{
				wenkong=1;
			}
			else if(RX1_BUFF[1]=='G')
			{
				wenkong=0;			
			}
			
			if(RX1_BUFF[2]=='K')//switch       K on G off
			{
				kaiguan=1;
				daojishi=dingshi*60;
			}
			else if(RX1_BUFF[2]=='G')
			{
				kaiguan=0;			
			}
		}		
				
		
		for(i=0;i<shumu;i++)
		{
		 RX1_BUFF[i]=0;
		}						
	}
}	
char MG4[43]={"100200300400$"};
/*----------------------------
Send string
----------------------------*/
void SendString(char *s)
{
    while (*s)                  //Detects end of string flag
    {
     USART1->DR = (*s++& (uint16_t)0x01FF);
		 while((USART1->SR&0X40)==0);
    }
}
//Interrupt priority configuration
void NVICConfig(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
 	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
		NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;	  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
//Timer setting
void TimerConfig(TIM_TypeDef* TIMx,unsigned int time)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_DeInit(TIMx);
  TIM_TimeBaseStructure.TIM_Period = time * 2;		 				/* Automatic reloading register cycle value (count value) */
																													/* Generate an update or interrupt after TIM_Period of frequency */
  TIM_TimeBaseStructure.TIM_Prescaler= (4000- 1);				    /* Clock predivision e.g. clock frequency =72MHZ/ (clock predivision +1) */
  TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 			/* Sampling frequency division */
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 		/* Upcount mode */
  TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure);
  TIM_ClearFlag(TIMx, TIM_FLAG_Update);							    /* Clear the overflow interrupt flag */
  TIM_ITConfig(TIMx,TIM_IT_Update,ENABLE);
}
//Switch of timer
void TimerSW( TIM_TypeDef* TIMx, FunctionalState NewState )
{
	 TIM_SetCounter(TIMx,0);	
   TIM_Cmd(TIMx,NewState);
}
void shangchuanshuju(void)
{
     //Upload data via Bluetooth
	   MG4[0]=wd%1000/100+'0';
	   MG4[1]=wd%100/10+'0';
	   MG4[2]=wd%10/1+'0';	
	   MG4[3]=sd%1000/100+'0';
	   MG4[4]=sd%100/10+'0';
	   MG4[5]=sd%10/1+'0';	
 	   MG4[6]=juli%1000/100+'0';
	   MG4[7]=juli%100/10+'0';
	   MG4[8]=juli%10/1+'0';
	   MG4[9]=dangwei%1000/100+'0';
	   MG4[10]=dangwei%100/10+'0';
	   MG4[11]=dangwei%10/1+'0';
	   MG4[12]=dingshi%1000/100+'0';
	   MG4[13]=dingshi%100/10+'0';
	   MG4[14]=dingshi%10/1+'0';	
	   MG4[15]=wenkong%10/1+'0';
	   MG4[16]=wenkong%10/1+'0';
	   MG4[17]=wenkong%10/1+'0';	
		 MG4[18]=wendusd%1000/100+'0';
	   MG4[19]=wendusd%100/10+'0';
	   MG4[20]=wendusd%10/1+'0';		
	   MG4[21]=kaiguan%10/1+'0';
	   MG4[22]=kaiguan%10/1+'0';
	   MG4[23]=kaiguan%10/1+'0';			 
		 
		 MG4[24]='$';		 
	   SendString(MG4);			
}
unsigned char rece_buf[32];
int main(void)
 {	

	 
	  //Initialize delay function
	  delay_init();	  
	  //Pin configuration
	  GPIOD_Init();
    //Initialize OLED  	 
		OLED_Init();			
		OLED_Clear(); 	
		//Initialize serial port one
		USART1_init();  
	  //Ultrasonic sensor configuration
    UltrasonicWave_Configuration();		 
	 	RCC_APB1PeriphClockCmd(	RCC_APB1Periph_TIM3, ENABLE );	//Enable GPIOA clock
	  NVICConfig();
	  TimerConfig(TIM3,1);
	  TimerSW(TIM3, ENABLE);				 
	  while(1)
		{	
			//Only when the switch is on, display
			if(kaiguan==1)
			{
	 	   //OLED display temperature, humidity, distance and so on
		   xianshi(wd,sd,juli,dangwei,dingshi,daojishi,wenkong,wendusd);	       				
			}
			else
			{
				OLED_Clear(); 
			}	
		 //The temperature is read by DS18B20
		 DHT11_receive(&sd,&wd);
		 UltrasonicWave_StartMeasure();		
		 juli = UltrasonicWave_Distance/10.0;	
     //If distance too small
     if(juli<15 )
 		 {
			 //Fan off
 			fskg=0;//			 
		 }
		 else
		 {
			//Fan on
 			fskg=1;//   			 
		 }	
	   //Button scanning
		 anjiansaomiao();		
     //Upload data via Bluetooth		
		 shangchuanshuju();   
     chuli1();	
		 //Processing data from Bluetooth
		 RX1_CHULI();		
     //Fan speed is controlled according to temperature and gear
		 //Temperature control on/off
     if(wenkong==0)//0 off   1 on	
  	 {
				if(dangwei==1)
				{
					PWM=4;
				}
				else if(dangwei==2)
				{
					PWM=7;					
				}
				else if(dangwei==3)
				{
					PWM=11;						
				} 				
		 }
		 else
		 {  
			  //Automatic temperature control
			 	if(wd>wendusd)
				{
					dangwei=2;
					PWM=7;
				}
				else
				{
					dangwei=1;
					PWM=4;					
				}
		 }	
  	}
		
	}

//Timer 3 interrupt service routine
void TIM3_IRQHandler(void)   //TIM3 interrupt
{
	static 	char ct=0;
	static 	int ct1=0;
	
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //Check whether the TIM3 update interrupt occurs
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //Clear the TIM3 update interrupt flag
	  
if(fskg&&kaiguan)
{	
		ct++;
    if(ct<PWM)	
		{
			PBout(15) = 1;
		}	
    else
		{
			PBout(15) = 0;			
		}	
    if(ct>10)
		{
			ct=0;
		}
	}
	else
	{
				PBout(15) = 0;			
	}
		 
		ct1++;
		if(ct1>900)
		{
			ct1=0;
			if(daojishi)
			{
			 daojishi--;
			}
			else
			{
				//Fan off
				if(dingshi)
				{
				 kaiguan=0;//Turn off the monitor when the countdown ends
				}
			}
		}

		
	}
}
