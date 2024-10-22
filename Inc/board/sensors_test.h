#ifndef _SENSORS_TEST_H_
#define _SENSORS_TEST_H_

#include "stm32l4xx_hal.h"

extern int isUpload;

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

#endif

