#include <string.h>
#include "app.h"
#include "usart.h"
#include "gpio.h"
#include "lorawan_node_driver.h"
#include "hdc1000.h"
#include "sensors_test.h"
#include "ST7789v.h"
#include "menu.h"


extern TIM_HandleTypeDef htim7;
extern DEVICE_MODE_T device_mode;
extern DEVICE_MODE_T *Device_Mode_str;
down_list_t *pphead = NULL;

float HUMI_MAX=90,HUMI_MIN=80,TEMP_MAX=37,TEMP_MIN=30;
float rt_temp,rt_humidi,rt_lux,rt_pressure; //�洢ʵʱ�¶Ⱥ�ʪ��
void offline_control(void);
void data_capture(void);

//-----------------Users application--------------------------
void LoRaWAN_Func_Process(void)
{
    static DEVICE_MODE_T dev_stat = NO_MODE;

    uint16_t temper = 0;

    switch((uint8_t)device_mode)
    {
    /* ָ��ģʽ */
    case CMD_CONFIG_MODE:
    {
        /* �������command Configuration function, �����if���,ִֻ��һ�� */
        if(dev_stat != CMD_CONFIG_MODE)
        {
            dev_stat = CMD_CONFIG_MODE;
            debug_printf("\r\n[Command Mode]\r\n");

            nodeGpioConfig(wake, wakeup);
            nodeGpioConfig(mode, command);
        }
        /* �ȴ�usart2�����ж� */
        if(UART_TO_PC_RECEIVE_FLAG)
        {
            UART_TO_PC_RECEIVE_FLAG = 0;
            lpusart1_send_data(UART_TO_PC_RECEIVE_BUFFER,UART_TO_PC_RECEIVE_LENGTH);
        }
        /* �ȴ�lpuart1�����ж� */
        if(UART_TO_LRM_RECEIVE_FLAG)
        {
            UART_TO_LRM_RECEIVE_FLAG = 0;
            usart2_send_data(UART_TO_LRM_RECEIVE_BUFFER,UART_TO_LRM_RECEIVE_LENGTH);
        }
    }
    break;

    /* ͸��ģʽ */
    case DATA_TRANSPORT_MODE:
    {
        /* �������data transport function,�����if���,ִֻ��һ�� */
        if(dev_stat != DATA_TRANSPORT_MODE)
        {
            dev_stat = DATA_TRANSPORT_MODE;
            debug_printf("\r\n[Transperant Mode]\r\n");

            /* ģ�������ж� */
            if(nodeJoinNet(JOIN_TIME_120_SEC) == false)
            {
                return;
            }

            temper = HDC1000_Read_Temper()/1000;

            nodeDataCommunicate((uint8_t*)&temper,sizeof(temper),&pphead);
        }

        /* �ȴ�usart2�����ж� */
        if(UART_TO_PC_RECEIVE_FLAG && GET_BUSY_LEVEL)  //Ensure BUSY is high before sending data
        {
            UART_TO_PC_RECEIVE_FLAG = 0;
            nodeDataCommunicate((uint8_t*)UART_TO_PC_RECEIVE_BUFFER, UART_TO_PC_RECEIVE_LENGTH, &pphead);
        }

        /* ���ģ����æ, ����������Ч��������������Ϣ */
        else if(UART_TO_PC_RECEIVE_FLAG && (GET_BUSY_LEVEL == 0))
        {
            UART_TO_PC_RECEIVE_FLAG = 0;
            debug_printf("--> Warning: Don't send data now! Module is busy!\r\n");
        }

        /* �ȴ�lpuart1�����ж� */
        if(UART_TO_LRM_RECEIVE_FLAG)
        {
            UART_TO_LRM_RECEIVE_FLAG = 0;
            usart2_send_data(UART_TO_LRM_RECEIVE_BUFFER,UART_TO_LRM_RECEIVE_LENGTH);
        }
    }
    break;

    /*����ģʽ*/
    case PRO_TRAINING_MODE:
    {
        /* �������Class C��ƽ̨���ݲɼ�ģʽ, �����if���,ִֻ��һ�� */
        if(dev_stat != PRO_TRAINING_MODE)
        {
            dev_stat = PRO_TRAINING_MODE;
            debug_printf("\r\n[Project Mode]\r\n");

        }
				
        /* ���ʵ�����λ�� */
        static uint8_t cycle_cnt=0;
        cycle_cnt++;
        if(cycle_cnt % 50 == 0){
            data_capture(); //���������ݲɼ�
        }
        
        offline_control(); //������ʪ�ȿ���

        MenuCmd(key_scan());
        if (navigate[cntpage]->dymantic_page)//���Ϊ��̬ҳ
        {
            MenuRender(0);
            HAL_Delay(100);
        }
        key_show(1);
        // HAL_TIM_Base_Start_IT(&htim7);

//        LCD_Clear(WHITE);
        // float temp = HDC1000_Read_Temper()*165.0/65536.0-40.0;
        // float Humi = HDC1000_Read_Humidi()*100/65536.0;
        // LCD_ShowString(0,0,"tempe:",BLACK);
        // LCD_ShowFloat(50,0,temp,2,3,BLACK);
        // debug_printf("\r\n[tempe:%.3f]\r\n",temp);//temp*(165.0/65536.0)-40
        // debug_printf("\r\n[Humi:%.3f]\r\n",Humi);	//Humi		
        // LCD_ShowNum(50,0,temp,5,BLACK);
        // delay_10ms(10);

    }
    break;

    default:
        break;
    }
}

void data_capture(void){
    rt_temp = HDC1000_Read_Temper()*165.0/65536.0-40.0;
    rt_humidi = HDC1000_Read_Humidi()*100/65536.0;
    uint16_t result;
    result = OPT3001_Result();
    rt_lux = 0.01*(1 << ((result & 0xF000) >> 12))*(result & 0xFFF);
    rt_pressure = MPL3115_ReadPressure();
}

void offline_control(void){
    if(rt_temp<TEMP_MIN){
        //�����������رշ��ȣ�reset�ǹر�
        HAL_GPIO_WritePin(LedGpio_D6, LedPin_D6, GPIO_PIN_SET);
        HAL_GPIO_WritePin(LedGpio_D7, LedPin_D7, GPIO_PIN_RESET);
    }else if(rt_temp>TEMP_MAX){
        HAL_GPIO_WritePin(LedGpio_D6, LedPin_D6, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(LedGpio_D7, LedPin_D7, GPIO_PIN_SET);
    }else{
        //���еĻ�ȫ�����رգ��ɻ�������������ʡ��
        HAL_GPIO_WritePin(LedGpio_D6, LedPin_D6, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(LedGpio_D7, LedPin_D7, GPIO_PIN_RESET);
    }

    if(rt_humidi<HUMI_MIN){
        //��������رշ���
        HAL_GPIO_WritePin(LedGpio_D8, LedPin_D8, GPIO_PIN_SET);
        HAL_GPIO_WritePin(LedGpio_D7, LedPin_D7, GPIO_PIN_RESET);
    }else if(rt_humidi>HUMI_MAX){
        HAL_GPIO_WritePin(LedGpio_D8, LedPin_D8, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(LedGpio_D7, LedPin_D7, GPIO_PIN_SET);
    }else{
        HAL_GPIO_WritePin(LedGpio_D8, LedPin_D8, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(LedGpio_D7, LedPin_D7, GPIO_PIN_RESET);
    }
}


/**
 * @brief   ������汾��Ϣ�Ͱ���ʹ��˵����Ϣ��ӡ
 * @details �ϵ����еƻ������100ms
 * @param   ��
 * @return  ��
 */
void LoRaWAN_Borad_Info_Print(void)
{
    debug_printf("\r\n\r\n");
    PRINT_CODE_VERSION_INFO("%s",CODE_VERSION);
    debug_printf("\r\n");
    debug_printf("--> Press Key1 to: \r\n");
    debug_printf("-->  - Enter command Mode\r\n");
    debug_printf("-->  - Enter Transparent Mode\r\n");
    debug_printf("--> Press Key2 to: \r\n");
    debug_printf("-->  - Enter Project Trainning Mode\r\n");
    LEDALL_ON;
    HAL_Delay(100);
    LEDALL_OFF;
}




