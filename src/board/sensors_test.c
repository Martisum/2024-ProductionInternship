#include "sensors_test.h"
#include "hdc1000.h"
#include "usart.h"
#include "opt3001.h"
#include "MPL3115.h"
#include "mma8451.h"
#include "ST7789v.h"
#include "menu.h"
#include "XPT2046.h"

extern Pen_Holder Pen_Point;//�����ʵ��
int8_t Error_num = 0;

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
            debug_printf("��ʪ�ȴ��������� �¶�: %.3f ��   ʪ��: %.3f%% \r\n",(double)temper/1000.0,(double)humi/1000.0);
        } else
        {
            Error_num = -13;
            debug_printf("��ʪ�ȴ������쳣  error:d% \r\n",Error_num);
        }
    } else
    {
        debug_printf("��ʪ�ȴ������쳣  error:d% \r\n",Error_num);
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
            debug_printf("�նȴ��������� �ն�: %.2f Lux \r\n",lux);
        } else
        {
            Error_num = -16;
            debug_printf("�նȴ������쳣  error:d% \r\n",Error_num);
        }
    } else
    {
        debug_printf("�նȴ������쳣  error:d% \r\n",Error_num);
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
            debug_printf("��ѹ���������� ��ѹ: %.2f Pa \r\n",pressure);
        } else
        {
            Error_num = -18;
            debug_printf("��ѹ�������쳣  error:d% \r\n",Error_num);
        }
    } else
    {
        debug_printf("��ѹ�������쳣  error:d% \r\n",Error_num);
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
            debug_printf("���ٶȴ��������� X: %d  Y: %d  Z: %d  \r\n",tAccel.accel_x,tAccel.accel_y,tAccel.accel_z);
        } else
        {
            Error_num = -20;
            debug_printf("���ٶȴ������쳣  error:d% \r\n",Error_num);
        }
    } else
    {
        debug_printf("���ٶȴ������쳣  error:d% \r\n",Error_num);
    }

    Error_num = 0;

}

void temper_test(void)
{
		float temper = 0;
		while(1)
		{
			temper = HDC1000_Read_Temper()*165.0/65536.0-40.0;
			LCD_ShowString(0,0,"temper:",BLACK);
			LCD_ShowFloat(60,0,temper,2,3,BLACK);
			LCD_Clear(WHITE);
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
			HAL_Delay(100);
		}    
}

void Humidi_test(void)
{
		float Humidi = 0;
		while(1)
		{
			Humidi = HDC1000_Read_Humidi()*100/65536.0;
			LCD_ShowString(0,0,"Humidi:",BLACK);
			LCD_ShowFloat(60,0,Humidi,2,3,BLACK);
			LCD_Clear(WHITE);
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
			HAL_Delay(100);
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

