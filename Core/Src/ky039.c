#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "stm32f4xx.h"
#include "adc.h"

uint32_t ky039_raw_value;

#define THRESHOLD 2000
#define SAMPLE_PERIOD_MS 10
#define MEASUREMENT_TIME_MS 1000  // 10 second window

static uint32_t last_value = 0;
static uint32_t beat_count = 0;
static uint32_t sample_counter = 0;

int32_t read_heart(void)
{
	ky039_raw_value = adc_read(0) * 500;  // ky09 -> PA0-> CH0

    // Detect rising edge
    if (ky039_raw_value > THRESHOLD && last_value <= THRESHOLD)
    {
        beat_count++;
    }
    last_value = ky039_raw_value;

    sample_counter += SAMPLE_PERIOD_MS;

    // Every MEASUREMENT_TIME_MS, compute BPM and reset
    if (sample_counter >= MEASUREMENT_TIME_MS)
    {
        int32_t bpm = beat_count * (6000 / MEASUREMENT_TIME_MS);  // 1 minute scale
        beat_count = 0;
        sample_counter = 0;
        return bpm;
    }

    return -1; // if bpm not ready
}
