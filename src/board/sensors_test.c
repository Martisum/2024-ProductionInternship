#include "sensors_test.h"
#include "hdc1000.h"
#include "usart.h"
#include "opt3001.h"
#include "MPL3115.h"
#include "mma8451.h"
#include "ST7789v.h"
#include "menu.h"
#include "XPT2046.h"
#include "app.h"
#include "tim.h"
#include "stdbool.h"
#include "stdio.h"

extern Pen_Holder Pen_Point;//定义笔实体
int8_t Error_num = 0;
uint8_t data_upload[20];
int isUpload=0;
int lux_set= 2500;
PID my_lux;
float lux_kp=1.0,lux_ki=0.4,lux_kd=0;

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
		my_lux.PS=lux_kp;
    my_lux.IS=lux_ki;
    my_lux.DS=lux_kd;
		while(1)
		{	
			LCD_Clear(WHITE);
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
			lux_pid(lux_set,lux);
			key_show(0);
			HAL_Delay(50);
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

void speculates(void)
{
		LCD_Clear(WHITE);

		//启用1号测试数据
		if(test_date==1){
			day_info[0].temp=26;	day_info[0].humidi=92;	day_info[0].pressure=901;
			day_info[1].temp=26;	day_info[1].humidi=92;	day_info[1].pressure=887;
			day_info[2].temp=26;	day_info[2].humidi=95;	day_info[2].pressure=897;
			day_info[3].temp=25;	day_info[3].humidi=97;	day_info[3].pressure=906;
			day_info[4].temp=25;	day_info[4].humidi=95;	day_info[4].pressure=918;
			day_info[5].temp=23;	day_info[5].humidi=94;	day_info[5].pressure=890;
			day_info[6].temp=23;	day_info[6].humidi=96;	day_info[6].pressure=891;
			day_info[7].temp=24;	day_info[7].humidi=92;	day_info[7].pressure=897;
			day_info[8].temp=24;	day_info[8].humidi=97;	day_info[8].pressure=895;
			day_info[9].temp=24;	day_info[9].humidi=97;	day_info[9].pressure=904;
			day_info[10].temp=25;	day_info[10].humidi=92;	day_info[10].pressure=903;
			day_info[11].temp=25;	day_info[11].humidi=90;	day_info[11].pressure=896;

			day_info[12].temp=25;	day_info[12].humidi=99;	day_info[12].pressure=897;
			day_info[13].temp=25;	day_info[13].humidi=96;	day_info[13].pressure=899;
			day_info[14].temp=25;	day_info[14].humidi=94;	day_info[14].pressure=902;
			day_info[15].temp=23;	day_info[15].humidi=94;	day_info[15].pressure=910;
			day_info[16].temp=23;	day_info[16].humidi=99;	day_info[16].pressure=913;
			day_info[17].temp=23;	day_info[17].humidi=94;	day_info[17].pressure=904;
			day_info[18].temp=23;	day_info[18].humidi=96;	day_info[18].pressure=904;
			day_info[19].temp=24;	day_info[19].humidi=92;	day_info[19].pressure=902;
			day_info[20].temp=24;	day_info[20].humidi=91;	day_info[20].pressure=896;
			day_info[21].temp=24;	day_info[21].humidi=90;	day_info[21].pressure=897;
			day_info[22].temp=24;	day_info[22].humidi=91;	day_info[22].pressure=890;
			day_info[23].temp=24;	day_info[23].humidi=92;	day_info[23].pressure=899;
		}

		while(1)
		{	
			static char tmp_str[30]={0}; //临时使用字符串
			static uint8_t cycle_cnt=0;
        	cycle_cnt++;
			if(cycle_cnt % 25 == 0){
				//雨晴推测数据打印
				LCD_Fill(10,10,160,100,WHITE);
        	}
			sprintf(tmp_str,"%02d/%02d/%02d",2000 + GetData.Year, GetData.Month, GetData.Date);
			LCD_ShowString(10,10,(const uint8_t *)tmp_str,BLACK);
			sprintf(tmp_str,"%02d:%02d",GetTime.Hours, GetTime.Minutes);
			LCD_ShowString(10,25,(const uint8_t *)tmp_str,BLACK);
			sprintf(tmp_str,"depression:%d",mark_depression);
			LCD_ShowString(10,40,(const uint8_t *)tmp_str,BLACK);
			sprintf(tmp_str,"high_humi:%d",mark_high_humi);
			LCD_ShowString(10,55,(const uint8_t *)tmp_str,BLACK);
			sprintf(tmp_str,"sud_temp_drop:%d",mark_sud_temp_drop);
			LCD_ShowString(10,70,(const uint8_t *)tmp_str,BLACK);
			sprintf(tmp_str,"rainy:%d",mark_rainy);
			LCD_ShowString(10,85,(const uint8_t *)tmp_str,BLACK);

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

		if(isUpload){
			debug_printf("\r\n[Transperant Mode]\r\n");
			/* 模块入网判断 */
			if(nodeJoinNet(JOIN_TIME_120_SEC) == false)
			{
				return;
			}
			temper = HDC1000_Read_Temper()/1000;
			nodeDataCommunicate((uint8_t*)&temper,sizeof(temper),&pphead);
		}
		

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

			HAL_GPIO_WritePin(GPIOE,GPIO_PIN_2,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOE,GPIO_PIN_3,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOE,GPIO_PIN_4,GPIO_PIN_RESET);
			
			if(isUpload){
				/* 等待usart2产生中断 */
				if(UART_TO_PC_RECEIVE_FLAG && GET_BUSY_LEVEL){
					uint8_t i=1;
					uint16_t raw_data[4];

					raw_data[0]=temper*100;//考虑小数点
					raw_data[1]=Humidi*100;
					raw_data[3]=lux;
					raw_data[4]=pressure;
					data_upload[0]=0xAA;
					for(i=1;i<=8;i++){
						if(i%2) data_upload[i]=(raw_data[i/2]>>8) & 0xff;
						else data_upload[i]=raw_data[i/2] & 0xff;
					}data_upload[9]=0xFF;

					//服务器上报
					UART_TO_PC_RECEIVE_FLAG = 0;
					// nodeDataCommunicate((uint8_t*)UART_TO_PC_RECEIVE_BUFFER, UART_TO_PC_RECEIVE_LENGTH, &pphead);
					nodeDataCommunicate((uint8_t*)data_upload, 10, &pphead);
				}else if(UART_TO_PC_RECEIVE_FLAG && (GET_BUSY_LEVEL == 0)){
					UART_TO_PC_RECEIVE_FLAG = 0;
					debug_printf("--> Warning: Don't send data now! Module is busy!\r\n");
				}
				if(UART_TO_LRM_RECEIVE_FLAG){
					UART_TO_LRM_RECEIVE_FLAG = 0;
					usart2_send_data(UART_TO_LRM_RECEIVE_BUFFER,UART_TO_LRM_RECEIVE_LENGTH);
				}
			}
			// LCD_ShowString(10,62,"Rain",BLACK);	
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

void lux_pid(int16_t luxset,float now_lux){

    my_lux.set_targetS=luxset;
    my_lux.now_error= my_lux.set_targetS - now_lux;

    my_lux.sum_error+=my_lux.now_error;
    if(my_lux.sum_error>20000) my_lux.sum_error=20000;
    else if(my_lux.sum_error<-20000) my_lux.sum_error=-20000;

    my_lux.pwm_out= 0.01*(my_lux.PS*my_lux.now_error+my_lux.DS*(my_lux.now_error-my_lux.pre_error)+my_lux.IS*my_lux.sum_error);
    if(my_lux.pwm_out>180) my_lux.pwm_out=180;
    else if(my_lux.pwm_out<0) my_lux.pwm_out= 0;
		
	
		LCD_ShowFloat(10,32,my_lux.pwm_out,4,2,BLACK);
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, my_lux.pwm_out);
    my_lux.pre_error=my_lux.now_error;
}

void pwmtest(void){
	float pwm = 0;
	while(1){
		LCD_Clear(WHITE);
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, pwm);
		if(Pen_Point.Key_Sta == 1)
		{
				Pen_Point.Key_Sta =0;
				float posx = Pen_Point.X*240.0/2048.0;
				float posy = 360-Pen_Point.Y*360.0/2048.0;
				if(posx>=170 && posx <=220 && posy>=280 && posy<=320)
			 {
				 __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);
				 MenuRender(1);
				 return;
			 }
		}
		key_show(0);
		pwm++;
		if(pwm>=200)pwm=0;
		LCD_ShowFloat(10,0,pwm,5,1,BLACK);	
		HAL_Delay(100);
	}
}
