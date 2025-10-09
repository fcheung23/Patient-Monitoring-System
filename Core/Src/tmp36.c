#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "stm32f4xx.h"
#include "adc.h"

int32_t read_tmp(void)
{
	while(1)
	{
		uint32_t tmp36_raw_value = adc_read(1); // tmp36 -> PA1-> CH1

		// 12-bit resolution, 3.3 V
		int32_t voltage_mV = (tmp36_raw_value * 3300) / 4096;

		// 10 mV/C, 500 mV at 0 C
		int32_t tempC_x10 = (voltage_mV - 500); // [(mV - 500) / 10] * 10

		return tempC_x10;
	}
}
