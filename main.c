#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"	
#include "usmart.h"	 
#include "rtc.h" 
#include "adc.h"
#include "lsens.h"
#include "24cxx.h"
#include "w25qxx.h"
#include "touch.h"
#include "timer.h"
#include <math.h>
#include "stmflash.h"

#define FIRST 0x08070000
#define MODEFLAG 0x08071000
#define SMONTH 0x08072000
#define EMONTH 0x08073000
#define STARTH 0x08074000
#define ENDH 0x08075000
#define STARTM 0x08076000
#define ENDM 0x08077000

/*
 Created by ShengOuY
 XMU Automation 2017
*/

u32 First;
u32 XLow[13]={190,10,70,130,10,70,130,10,70,130,190,250,190};
u32 XHigh[13]={250,70,130,190,70,130,190,70,130,190,250,310,310};
u32 YLow[13]={240,240,240,240,320,320,320,400,400,400,320,240,400};
u32 YHigh[13]={320,320,320,320,400,400,400,480,480,480,400,400,480};
u32 Touchx=0;
u32 Touchy=0;
u8 ModeFlag; //0Ϊ�ƹ���ƣ�1Ϊʱ�����
u8 SeasonFlag=0;//0Ϊ�ļ�ʱ��1Ϊ����ʱ
u8 LampFlag=0; //0Ϊ�رգ�1Ϊ����
//���������־
u8 set=0; //0Ϊδ��������״̬
u8 setFlag=0; //0Ϊδ��������ѡ��1Ϊѡ��mode��2Ϊѡ��season��3data��4time
u8 MinLight=30;
u8 MaxLight=60;
u8 LastLight=0;
u8 StartH[2];
u8 EndH[2];
u8 StartM[2];
u8 EndM[2];

u8 Smonth;
u8 Emonth;

u32 SetYear=0;
u32 SetMon=0;
u32 SetDay=0;
u32 SetHour=0;
u32 SetMin=0;
u32 SetSec=0;

u8 CountY=0;
u8 CountM=0;
u8 CountD=0;
u8 Counth=0;
u8 Countm=0;
u8 Counts=0;

u8 scount=0;
u8 ecount=0;
u8 temps=0;
u8 tempe=0;


void initShow()
{
	POINT_COLOR=BLACK;//��������Ϊ��ɫ
	LCD_ShowString(60,10,200,16,16,"created by ShengOuYang");
	POINT_COLOR=BLUE;//��������Ϊ��ɫ
	LCD_DrawLine(0,30,320,30);
	LCD_ShowString(20,40,140,16,16,"Lamp State:**");	
	LCD_ShowString(160,40,80,16,16,"Mode:*");
	//��ʾʱ��
	LCD_ShowString(20,60,280,16,16,"Data: ****-**-**");
	LCD_ShowString(20,80,200,16,16,"Time: **:**:**");	
	LCD_ShowString(160,80,80,16,16,"Season:*");
	//light
	LCD_ShowString(20,100,200,16,16,"Light Intensity:");
	
	LCD_DrawLine(0,116,320,116);
	LCD_DrawLine(0,240,320,240);
	
	//��ʾ����
	LCD_DrawLine(10,320,250,320);
	LCD_DrawLine(10,400,310,400);
	LCD_DrawLine(10,480,310,480);
	
	LCD_DrawLine(10,240,10,480);
	LCD_DrawLine(70,240,70,480);
	LCD_DrawLine(130,240,130,480);
	LCD_DrawLine(190,240,190,480);
	LCD_DrawLine(250,240,250,400);
	LCD_DrawLine(310,240,310,480);
	
	POINT_COLOR=BLACK;//��������Ϊ��ɫ
	LCD_ShowChar(30,270,'1',24,0);
	LCD_ShowChar(90,270,'2',24,0);
	LCD_ShowChar(150,270,'3',24,0);
	LCD_ShowChar(210,270,'0',24,0);
	LCD_ShowChar(270,270,'S',24,0);
	
	LCD_ShowChar(30,350,'4',24,0);
	LCD_ShowChar(90,350,'5',24,0);
	LCD_ShowChar(150,350,'6',24,0);
	LCD_ShowString(200,350,40,24,24,"Del");
	LCD_ShowChar(270,300,'E',24,0);
	
	LCD_ShowChar(30,430,'7',24,0);
	LCD_ShowChar(90,430,'8',24,0);
	LCD_ShowChar(150,430,'9',24,0);
	LCD_ShowString(240,430,40,24,24,"OK");
	LCD_ShowChar(270,330,'T',24,0);
	
//	LCD_ShowNum(20,120,First,4,16);
//	LCD_ShowNum(20,140,ModeFlag,1,16);
	
}

void showTime(u8 t)
	{
			POINT_COLOR=BLACK;//  
			if(t!=calendar.sec||1)
				{
					LCD_ShowNum(68,60,calendar.w_year,4,16);									  
					LCD_ShowNum(108,60,calendar.w_month,2,16);									  
					LCD_ShowNum(132,60,calendar.w_date,2,16);	 
					switch(calendar.week)
					{
						case 0:
							LCD_ShowString(180,60,80,16,16,"Sunday   ");
							break;
						case 1:
							LCD_ShowString(180,60,80,16,16,"Monday   ");
							break;
						case 2:
							LCD_ShowString(180,60,80,16,16,"Tuesday  ");
							break;
						case 3:
							LCD_ShowString(180,60,80,16,16,"Wednesday");
							break;
						case 4:
							LCD_ShowString(180,60,80,16,16,"Thursday ");
							break;
						case 5:
							LCD_ShowString(180,60,80,16,16,"Friday   ");
							break;
						case 6:
							LCD_ShowString(180,60,80,16,16,"Saturday ");
							break;  
					}
					LCD_ShowNum(68,80,calendar.hour,2,16);									  
					LCD_ShowNum(92,80,calendar.min,2,16);									  
					LCD_ShowNum(116,80,calendar.sec,2,16);
				}	
	}
	
	void showLight()
	{
		u8 adcx;
		adcx=Lsens_Get_Val();
		LCD_ShowxNum(180,100,adcx,3,16,0);//��ʾADC��ֵ 
		
	}
	
	
	void showState()
	{
		if(LampFlag==0)
		{
			LED0=1;//�ر�
			LCD_ShowString(108,40,140,16,16,"Off");
		}else
		{
			LED0=0;//��
			LCD_ShowString(108,40,140,16,16," On");
		}
		if(calendar.w_month>=Smonth&&calendar.w_month<=Emonth)
		{
			LCD_ShowString(216,80,100,16,16,"Summer");
			SeasonFlag=0;
		}
		else{
			LCD_ShowString(216,80,100,16,16,"Winter");
		}
		if(ModeFlag==0){
			LCD_ShowString(200,40,100,16,16,"Light");
		}else{
			LCD_ShowString(200,40,100,16,16," Time");
		}
	
	}
	
u8 keyTest(u32 xu,u32 yu,u32 xd,u32 yd)
	{
		if(Touchx<320&&Touchy<480)
			{	
				if((xu<Touchx&&Touchx<xd)&&(yu<Touchy&&Touchy<yd))
				{
//					LCD_ShowString(20,120,200,16,16,"Key:   is Pressed!");
//					LCD_ShowNum(20,140,lcddev.width,3,16);	
//					LCD_ShowNum(20,160,lcddev.height,3,16);	
//					LCD_ShowNum(20,180,tp_dev.x[0],3,16);	
//					LCD_ShowNum(20,200,tp_dev.y[0],3,16);
					return 1;
				}		   				
		}
		return 0;
	}
	
	u8 keyReturn()
	{
		int i;
		for(i=0;i<13;i++)
		{
			if(keyTest(XLow[i],YLow[i],XHigh[i],YHigh[i])){
				return i;
			}
		}
		return 15;
	}
	
	void initFlag()
	{
		set=0;
		setFlag=0;
		CountY=0;
		CountM=0;
		CountD=0;
		SetYear=0;
		SetMon=0;
		SetDay=0;
		SetHour=0;
		SetMin=0;
		SetSec=0;
		Counth=0;
    Countm=0;
    Counts=0;
		scount=0;
		ecount=0;
		temps=0;
		tempe=0;

	}

			//Flash�ϵ籣��
		void saveToFlash()
		{
			FLASH_Unlock();
			FLASH_ErasePage(FIRST);
			FLASH_ErasePage(MODEFLAG);
			FLASH_ErasePage(SMONTH);
			FLASH_ErasePage(EMONTH);
			FLASH_ErasePage(STARTH);
			FLASH_ErasePage(ENDH);
			FLASH_ErasePage(STARTM);
			FLASH_ErasePage(ENDM);
			
			FLASH_ProgramWord(FIRST,First);
			FLASH_ProgramWord(MODEFLAG,ModeFlag);
			FLASH_ProgramWord(SMONTH,Smonth);
			FLASH_ProgramWord(EMONTH,Emonth);
			
			
			FLASH_ProgramWord(STARTH,*StartH);
			FLASH_ProgramWord(ENDH,*EndH);
			FLASH_ProgramWord(STARTM,*StartM);
			FLASH_ProgramWord(ENDM,*EndM);
			FLASH_Lock();
		}
	
//��ʾ������Ϣ	
	void showInfo(u8 n)
	{
		LCD_Fill(10,120,320,239,WHITE);
		if(n==1)
		{
			LCD_ShowString(20,120,280,16,16,"Choose num to set,press Del to exit!");
			LCD_ShowString(20,140,280,16,16,"1: Mode");
			LCD_ShowString(20,160,280,16,16,"2: Season");
			LCD_ShowString(20,180,280,16,16,"3: Data");
			LCD_ShowString(20,200,280,16,16,"4: Time");
		}
		else if(n==2)
		{
			LCD_ShowString(20,120,280,16,16,"Exit successfully!");
			initFlag();
			
		}
		else if(n==3)
		{
			LCD_ShowString(20,120,280,16,16,"Wrong num!");
			initFlag();
		}
		else if(n==4)
		{
			LCD_ShowString(20,120,300,16,16,"Choose a mode,press Del to exit!");
			LCD_ShowString(20,140,280,16,16,"1: Light");
			LCD_ShowString(20,160,280,16,16,"2: Time");
		}
		else if(n==5)
		{
			saveToFlash();
			LCD_ShowString(20,120,280,16,16,"Set successfully!");
			initFlag();
		}
		else if(n==6)
		{
			LCD_ShowString(20,120,300,16,16,"Set summer Month,press Del to exit!");
			LCD_ShowString(20,140,280,16,16,"1: StartMonth: **");
			LCD_ShowString(20,160,280,16,16,"2: EndMonth: **");
		}
		else if(n==7)
		{
			LCD_ShowString(20,120,280,16,16,"Set data:****-**-**!");
		}
		else if(n==8)
		{
			LCD_ShowString(20,120,280,16,16,"Set time:**:**:**!");
		}
	}
	
	//�޸�ģʽ
	void handleMode()
 {
	 setFlag=1;
	 showInfo(4);
 }
	
	
	//�޸�ʱ��
	void handleSeason()
  {
		setFlag=2;
	  showInfo(6);
  }
	
	
	
	//�޸�����
	void handleData()
  {
	 setFlag=3;
	 showInfo(7);
  }
	
	
  //�޸�ʱ��
	void handleTime()
  {
	  setFlag=4;
	  showInfo(8);
		
  }
	
	//��������
	void handleSet()
	{
			showInfo(1);
			set=1;
		}
		
	//��������
		void setData(u8 key)
		{
			if(key==12||CountY+CountM+CountD==8)
			{
				calendar.w_year=SetYear;
				calendar.w_month=SetMon;
				calendar.w_date=SetDay;
				RTC_ITConfig(RTC_IT_SEC, ENABLE);		//ʹ��RTC���ж�
				RTC_WaitForLastTask();	//�ȴ����һ�ζ�RTC�Ĵ�����д�������
		    RTC_EnterConfigMode();/// ��������	
				RTC_WaitForLastTask();	//�ȴ����һ�ζ�RTC�Ĵ�����д�������
		    RTC_Set(SetYear,SetMon,SetDay,calendar.hour,calendar.min,calendar.sec);  //����ʱ��	
				RTC_ExitConfigMode(); //�˳�����ģʽ 
				BKP_WriteBackupRegister(BKP_DR1, 0X5050);	//��ָ���ĺ󱸼Ĵ�����д���û���������
				initFlag();
				return;
			}
			if(CountY<4)
			{
				SetYear+=key*pow(10,3-CountY);
				CountY++;
				LCD_Fill(10,120,320,239,WHITE);
				LCD_ShowString(20,120,280,16,16,"Set Year:");
				LCD_ShowNum(100,120,SetYear,4,16);
			}else if(CountM<2)
			{
				SetMon+=key*pow(10,1-CountM);
				CountM++;
				LCD_Fill(10,120,320,239,WHITE);
				LCD_ShowString(20,140,280,16,16,"Set Month:");
				LCD_ShowNum(100,140,SetMon,2,16);
			}else if(CountD<2)
			{
				SetDay+=key*pow(10,1-CountD);
				CountD++;
				LCD_Fill(10,120,320,239,WHITE);
				LCD_ShowString(20,160,280,16,16,"Set Day:");
				LCD_ShowNum(100,160,SetDay,2,16);
			}
		}
		
		//����ʱ��
		void setTime(u8 key)
		{
			if(key==12||Counth+Countm+Counts==6)
			{
				RTC_ITConfig(RTC_IT_SEC, ENABLE);		//ʹ��RTC���ж�
				RTC_WaitForLastTask();	//�ȴ����һ�ζ�RTC�Ĵ�����д�������
		    RTC_EnterConfigMode();/// ��������	
				RTC_WaitForLastTask();	//�ȴ����һ�ζ�RTC�Ĵ�����д�������
		    RTC_Set(calendar.w_year,calendar.w_month,calendar.w_date,SetHour,SetMin,SetSec);  //����ʱ��	
				RTC_ExitConfigMode(); //�˳�����ģʽ 
				BKP_WriteBackupRegister(BKP_DR1, 0X5050);	//��ָ���ĺ󱸼Ĵ�����д���û���������
				initFlag();
				return;
			}
			if(Counth<2)
			{
				SetHour+=key*pow(10,1-Counth);
				Counth++;
				LCD_Fill(10,120,320,239,WHITE);
				LCD_ShowString(20,120,280,16,16,"Set Hour:");
				LCD_ShowNum(100,120,SetHour,2,16);
			}else if(Countm<2)
			{
				SetMin+=key*pow(10,1-Countm);
				Countm++;
				LCD_Fill(10,120,320,239,WHITE);
				LCD_ShowString(20,140,280,16,16,"Set Min:");
				LCD_ShowNum(100,140,SetMin,2,16);
			}else if(Counts<2)
			{
				SetSec+=key*pow(10,1-Counts);
				Counts++;
				LCD_Fill(10,120,320,239,WHITE);
				LCD_ShowString(20,160,280,16,16,"Set Sec:");
				LCD_ShowNum(100,160,SetSec,2,16);
			}
		}
		
//		u8 scount=0;
//		u8 ecount=0;
//		u8 temps;
//		u8 tempe;
		void setSeason(u8 key)
		{
			if(key==12||scount+ecount==4)
			{
				Smonth=temps;
				Emonth=tempe;
				initFlag();
				return;
			}
			else if(scount<2)
			{
				temps+=key*pow(10,1-scount);
				scount++;
				LCD_Fill(10,120,320,239,WHITE);
				LCD_ShowString(20,140,280,16,16,"Set SMonth:");
				LCD_ShowNum(120,140,temps,2,16);
			}else if(ecount<2)
			{
				tempe+=key*pow(10,1-ecount);
				ecount++;
				LCD_Fill(10,120,320,239,WHITE);
				LCD_ShowString(20,160,280,16,16,"Set Emonth:");
				LCD_ShowNum(120,160,tempe,2,16);
			}
		}
		
		
		

	//��ʱ��3�жϷ������ �����������¼�
void TIM3_IRQHandler(void)   //TIM3�ж�
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //���TIM3�����жϷ������
		{
			u8 key=14;
			//saveToFlash();
		  TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //���TIMx�����жϱ�־ 
			tp_dev.scan(0);
			//delay_ms(10);
			if(tp_dev.sta&TP_PRES_DOWN)	{		//������������
				 LCD_Fill(10,120,320,239,WHITE);
			   Touchx=tp_dev.x[0];
		     Touchy=tp_dev.y[0];
			   key=keyReturn();
			   //LCD_ShowNum(48,120,key,2,16);
			   if(key==11&&set==0)
			   {
				   handleSet();
			   }else if(set==1&&setFlag==0)
				 {
					   switch(key)
						{
							case 10:
								showInfo(2);
								break;
							case 1:
								handleMode();//
								break;
							case 2:
								handleSeason();//
								break;
							case 3:
								handleData();//
								break;
							case 4:
								handleTime();//
								break;
							default:
								showInfo(3);
						
						}
				 }else if(set==1&&setFlag==1)
				 {
					 switch(key)
						{
							case 10:
								showInfo(2);
								break;
							case 1:
								ModeFlag=0;//
								showInfo(5);
								break;
							case 2:
								ModeFlag=1;//
								showInfo(5);
								break;
							default:
								showInfo(3);
						}
				 }else if(set==1&&setFlag==2)
				 {
					 switch(key)
						{
							case 10:
								showInfo(2);
								break;
							case 11:
								showInfo(5);
								break;
							default:
								setSeason(key);
						}
				 }else if(set==1&&setFlag==3)
				 {
					 switch(key)
						{
							case 10:
								showInfo(2);
								break;
							case 11:
								showInfo(5);
								break;
							default:
								setData(key);
						}
				 }else if(set==1&&setFlag==4)
				 {
					 switch(key)
						{
							case 10:
								showInfo(2);
								break;
							case 11:
								showInfo(5);
								break;
							default:
								setTime(key);
						}
				 }
				 
			 }
			}
}
	
void lightControl()
{
	u8 adcx;
	adcx=Lsens_Get_Val();
	if(adcx>MaxLight||(LastLight>MinLight&&adcx>LastLight))
	{
		LampFlag=0;
		LastLight=adcx;
	}
	else
	{
		LampFlag=1;
		LastLight=adcx;
	}
}

void timeControl()
{
	if(calendar.hour>(EndH[SeasonFlag]+1)||calendar.hour<StartH[SeasonFlag])
	{
		LampFlag=0;
	}
	else if(calendar.hour==EndH[SeasonFlag]&&calendar.min>EndM[SeasonFlag])
	{
		LampFlag=0;
	}
	else if(calendar.hour==StartH[SeasonFlag]&&calendar.min<StartM[SeasonFlag])
	{
		LampFlag=0;
	}
	else
	{
		LampFlag=1;
	}
}	


//����·������ģʽ
void modeSet()
	{
		if(!ModeFlag)//��ǿ����ģʽ
		{
			lightControl();
		}
		else //ʱ�����ģʽ
		{
			timeControl();
		}
	}

	//Flash���ݳ�ʼ��
	initFlash()
	{
		First=*(u32*)FIRST;
		ModeFlag=*(u8*)MODEFLAG;
		Smonth=*(u8*)SMONTH;
		Emonth=*(u8*)EMONTH;
		*StartH=*(u8*)STARTH;
		*EndH=*(u8*)ENDH;
		*StartM=*(u8*)STARTM;
		*EndM=*(u8*)ENDM;
		if(First!=1401)
		{
			First=1401;
			ModeFlag=0;
			Smonth=4;
			Emonth=10;
			StartH[0]=19;
			StartH[1]=18;;
			EndH[0]=5;
			EndH[1]=6;
			StartM[0]=StartM[1]=EndM[0]=EndM[1]=30;
			
			FLASH_Unlock();
			FLASH_ErasePage(FIRST);
			FLASH_ErasePage(MODEFLAG);
			FLASH_ErasePage(SMONTH);
			FLASH_ErasePage(EMONTH);
			FLASH_ErasePage(STARTH);
			FLASH_ErasePage(ENDH);
			FLASH_ErasePage(STARTM);
			FLASH_ErasePage(ENDM);
			FLASH_ProgramWord(FIRST,First);
			FLASH_ProgramWord(MODEFLAG,ModeFlag);
			FLASH_ProgramWord(SMONTH,Smonth);
			FLASH_ProgramWord(EMONTH,Emonth);
			
			
			FLASH_ProgramWord(STARTH,*StartH);
			FLASH_ProgramWord(ENDH,*EndH);
			FLASH_ProgramWord(STARTM,*StartM);
			FLASH_ProgramWord(ENDM,*EndM);
			FLASH_Lock();
		}
		
	}
	
	
 int main(void)
 {
	u8 t=0;	
	initFlash();
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200
 	LED_Init();			     //LED�˿ڳ�ʼ��
	LCD_Init();
	usmart_dev.init(SystemCoreClock/1000000);	//��ʼ��USMART	
	RTC_Init();	  			//RTC��ʼ��
	Lsens_Init(); 			//��ʼ������������
	Adc3_Init();        //��ʼ��ADC
	tp_dev.init();
	initShow();
	TIM3_Int_Init(19,7199);//10Khz�ļ���Ƶ�ʣ�������500Ϊ50ms 
	
	while(1)
	{
		showTime(t);
		showLight();
		showState();
		modeSet();
		t=calendar.sec;
		delay_ms(10);
	}
	}  

