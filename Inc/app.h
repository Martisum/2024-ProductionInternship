#ifndef __APP_H
#define __APP_H
#include "stdint.h"
#include "lorawan_node_driver.h"

typedef struct enviro_info_struct {
    float temp;
    float humidi;
    float lux;
    float pressure;
} info_struct;

extern float HUMI_MAX,HUMI_MIN,TEMP_MAX,TEMP_MIN;
extern down_list_t *pphead;
extern info_struct day_info[24];
extern RTC_DateTypeDef GetData;  //获取日期结构体
extern RTC_TimeTypeDef GetTime;   //获取时间结构体
extern uint8_t mark_depression,mark_high_humi,mark_sud_temp_drop,mark_rainy;
extern uint8_t test_date;

/* 软件版本，升级时修改该信息即可 */
#define CODE_VERSION "V1.0.3"
/* 软件版本，升级时修改该信息即可 */
#define PRINT_CODE_VERSION_INFO(format, ...)  debug_printf("******** ---Based on LoRaWAN sensor data transmission experiment "format"_%s %s --- ********\r\n", ##__VA_ARGS__, __DATE__, __TIME__)


/** 串口接收内容长度最大值 512字节 */
#define USART_RECV_LEN_MAX 512

/** 入网超时时间：120s */
#define JOIN_TIME_120_SEC  120

typedef struct {
    char upcnt[10];
    char ackcnt[10];
    char toa[10];
    char nbt[10];

    char ch[10];
    char sf[10];
    char pwr[10];
    char per[10];

    char rssi[10];
    char snr[10];

} DEBUG;

void LoRaWAN_Func_Process(void);
void LoRaWAN_Borad_Info_Print(void);




#endif



