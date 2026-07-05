#ifndef __BSP_INTEMPER_ADC_H
#define __BSP_INTEMPER_ADC_H


#include "bsp_system.h"


void Printf_Logo(void);
uint32_t adc3_get_result(void);
int32_t adc3_get_temperature(void);
float adc_get_temperature_average(uint8_t times);


#endif


