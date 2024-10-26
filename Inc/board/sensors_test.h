#ifndef _SENSORS_TEST_H_
#define _SENSORS_TEST_H_

#include "stm32l4xx_hal.h"

extern int isUpload;
extern int lux_set;
extern float lux_kp,lux_ki,lux_kd;
typedef struct {
    int16_t set_targetS;
    int16_t pre_targetS;

    int PS;
    float IS;
    float DS;

    int16_t now_error;
    int16_t sum_error;
    int16_t pre_error;
    int diff_tar;

    int16_t pwm_out;
} PID;

void Test_task(void);
void HDC1000_Test(void);
void OPT3001_Test(void);
void MPL3115_Test(void);
void MMA8451_Test(void);
void LORA_NODE_Test(void);
void temper_test(void);
void Humidi_test(void);
void lux_test(void);
void pressure_test(void);
void FULL_test(void);
void lux_pid(int16_t lux,float now_lux);
void pwmtest(void);
void speculates(void);

#endif

