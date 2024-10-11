#include "sensors_test.h"
#include "hdc1000.h"
#include "usart.h"
#include "opt3001.h"
#include "MPL3115.h"
#include "mma8451.h"
#include "ST7789v.h"
#include "menu.h"
#include "XPT2046.h"

extern Pen_Holder Pen_Point;//定义笔实体
int8_t Error_num = 0;
uint8_t mark_depression=0,mark_high_humi=0,mark_sud_temp_drop=0,mark_rainy=0;

void Test_task(void)
{
    HDC1000_Test();
    OPT3001_Test();
    MPL3115_Test();
    MMA8451_Test();
    LORA_NODE_Test();
}

void HDC1000_Test(void)
{
    uint16_t temper, humi;

    temper = HDC1000_Read_Temper();
    humi = HDC1000_Read_Humidi();

    if(Error_num == 0)
    {
        if(temper !=0 || humi!=0)
        {
            debug_printf("温湿度传感器正常 温度: %.3f ℃   湿度: %.3f%% \r\n",(double)temper/1000.0,(double)humi/1000.0);
        } else
        {
            Error_num = -13;
            debug_printf("温湿度传感器异常  error:d% \r\n",Error_num);
        }
    } else
    {
        debug_printf("温湿度传感器异常  error:d% \r\n",Error_num);
    }

    Error_num = 0;
}

void OPT3001_Test(void)
{
    float lux;
    uint16_t result;

    result = OPT3001_Result();

    lux = 0.01*(1 << ((result & 0xF000) >> 12))*(result & 0xFFF);

    if(Error_num == 0)
    {
        if(lux !=0 )
        {
            debug_printf("照度传感器正常 照度: %.2f Lux \r\n",lux);
        } else
        {
            Error_num = -16;
            debug_printf("照度传感器异常  error:d% \r\n",Error_num);
        }
    } else
    {
        debug_printf("照度传感器异常  error:d% \r\n",Error_num);
    }

    Error_num = 0;

}

void MPL3115_Test(void)
{
    float pressure;

    pressure = MPL3115_ReadPressure();

    if(Error_num == 0)
    {
        if(pressure !=0 )
        {
            debug_printf("气压传感器正常 气压: %.2f Pa \r\n",pressure);
        } else
        {
            Error_num = -18;
            debug_printf("气压传感器异常  error:d% \r\n",Error_num);
        }
    } else
    {
        debug_printf("气压传感器异常  error:d% \r\n",Error_num);
    }

    Error_num = 0;
}


void MMA8451_Test(void)
{
    ACCELER_T tAccel;

    tAccel.accel_x = 999;
    tAccel.accel_y = 999;
    tAccel.accel_z = 999;

    tAccel = MMA8451_ReadAcceleration();

    if(Error_num == 0)
    {
        if(tAccel.accel_x !=999 || tAccel.accel_y !=999)
        {
            debug_printf("加速度传感器正常 X: %d  Y: %d  Z: %d  \r\n",tAccel.accel_x,tAccel.accel_y,tAccel.accel_z);
        } else
        {
            Error_num = -20;
            debug_printf("加速度传感器异常  error:d% \r\n",Error_num);
        }
    } else
    {
        debug_printf("加速度传感器异常  error:d% \r\n",Error_num);
    }

    Error_num = 0;

}

void temper_test(void)
{
		float temper = 0;
		LCD_Clear(WHITE);
		while(1)
		{		
			LCD_Cleardata(70,240,0,20);
			temper = HDC1000_Read_Temper()*165.0/65536.0-40.0;
			LCD_ShowString(10,0,"temper:",BLACK);
			LCD_ShowFloat(70,0,temper,2,3,BLACK);	
			if(Pen_Point.Key_Sta == 1)
			{
					Pen_Point.Key_Sta =0;
				  float posx = Pen_Point.X*240.0/2048.0;
					float posy = 360-Pen_Point.Y*360.0/2048.0;
					if(posx>=170 && posx <=220 && posy>=280 && posy<=320)
				 {
					 MenuRender(1);
					 return;
				 }
			}
			key_show(0);
			HAL_Delay(500);
		}    
}

void Humidi_test(void)
{
		float Humidi = 0;
		LCD_Clear(WHITE);
		while(1)
		{	
			LCD_Cleardata(70,240,0,20);
			Humidi = HDC1000_Read_Humidi()*100/65536.0;
			LCD_ShowString(10,0,"Humidi:",BLACK);
			LCD_ShowFloat(70,0,Humidi,2,3,BLACK);	
			if(Pen_Point.Key_Sta == 1)
			{
					Pen_Point.Key_Sta =0;
				  float posx = Pen_Point.X*240.0/2048.0;
					float posy = 360-Pen_Point.Y*360.0/2048.0;
					if(posx>=170 && posx <=220 && posy>=280 && posy<=320)
				 {
					 MenuRender(1);
					 return;
				 }
			}
			key_show(0);
			HAL_Delay(500);
		}    
}
void lux_test(void)
{
		float lux = 0;
		LCD_Clear(WHITE);
		while(1)
		{	
			LCD_Cleardata(42,240,0,20);
			uint16_t result;
			result = OPT3001_Result();
			lux = 0.01*(1 << ((result & 0xF000) >> 12))*(result & 0xFFF);
			LCD_ShowString(10,0,"lux:",BLACK);
			LCD_ShowFloat(42,0,lux,4,3,BLACK);	
			if(Pen_Point.Key_Sta == 1)
			{
					Pen_Point.Key_Sta =0;
				  float posx = Pen_Point.X*240.0/2048.0;
					float posy = 360-Pen_Point.Y*360.0/2048.0;
					if(posx>=170 && posx <=220 && posy>=280 && posy<=320)
				 {
					 MenuRender(1);
					 return;
				 }
			}
			key_show(0);
			HAL_Delay(500);
		}    
}
void pressure_test(void)
{
		float pressure = 0; 
		LCD_Clear(WHITE);
		while(1)
		{	
			LCD_Cleardata(82,240,0,24);
			pressure = MPL3115_ReadPressure();
			LCD_ShowString(10,0,"pressure:",BLACK);
			LCD_ShowFloat(82,0,pressure,5,1,BLACK);	
			if(Pen_Point.Key_Sta == 1)
			{
					Pen_Point.Key_Sta =0;
				  float posx = Pen_Point.X*240.0/2048.0;
					float posy = 360-Pen_Point.Y*360.0/2048.0;
					if(posx>=170 && posx <=220 && posy>=280 && posy<=320)
				 {
					 MenuRender(1);
					 return;
				 }
			}
			key_show(0);
			HAL_Delay(500);
		}    
}
void FULL_test(void)
{
		float temper = 0;
		float Humidi = 0;
		float lux = 0;
		float pressure = 0; 
		LCD_Clear(WHITE);
		while(1)
		{
			LCD_Cleardata(40,240,0,80);
			temper = HDC1000_Read_Temper()*165.0/65536.0-40.0;
			Humidi = HDC1000_Read_Humidi()*100/65536.0;
			uint16_t result;
			result = OPT3001_Result();
			lux = 0.01*(1 << ((result & 0xF000) >> 12))*(result & 0xFFF);
			pressure = MPL3115_ReadPressure();
			LCD_ShowString(10,0,"temper:",BLACK);
			LCD_ShowFloat(70,0,temper,2,3,BLACK);	
			LCD_ShowString(120,0,"C",BLACK);
			LCD_ShowString(10,16,"Humidi:",BLACK);
			LCD_ShowFloat(70,16,Humidi,2,3,BLACK);
			LCD_ShowString(120,16,"%",BLACK);			
			LCD_ShowString(10,32,"lux:",BLACK);
			LCD_ShowFloat(42,32,lux,4,3,BLACK);	
			LCD_ShowString(10,48,"pressure:",BLACK);
			LCD_ShowFloat(82,48,pressure,5,1,BLACK);
			

			if(temper>40){
				HAL_GPIO_WritePin(GPIOE,GPIO_PIN_2,GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOE,GPIO_PIN_4,GPIO_PIN_SET);
			}
			else if(temper<30){
				HAL_GPIO_WritePin(GPIOE,GPIO_PIN_2,GPIO_PIN_SET);
				HAL_GPIO_WritePin(GPIOE,GPIO_PIN_4,GPIO_PIN_RESET);
			}

			if(Humidi>90){
				HAL_GPIO_WritePin(GPIOE,GPIO_PIN_3,GPIO_PIN_RESET);
			}else if(Humidi<80){
				HAL_GPIO_WritePin(GPIOE,GPIO_PIN_3,GPIO_PIN_SET);
			}
			
			if(Pen_Point.Key_Sta == 1)
			{
					Pen_Point.Key_Sta =0;
				  float posx = Pen_Point.X*240.0/2048.0;
					float posy = 360-Pen_Point.Y*360.0/2048.0;
					if(posx>=170 && posx <=220 && posy>=280 && posy<=320)
				 {
					 MenuRender(1);
					 return;
				 }
			}
			key_show(0);
			HAL_Delay(250);
		}
}

void LORA_NODE_Test(void)
{
#if 0
    GET_LoraNode_Info(LoRa_Node_str);

    Printf_LoRaNode_Info(LoRa_Node);

    LoraNode_SetGPIO(255,0);
    HAL_Delay(1000);
    LoraNode_SetGPIO(255,1);
    LoraNode_Save();
#endif
}

