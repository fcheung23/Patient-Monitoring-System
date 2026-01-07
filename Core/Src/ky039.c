#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "stm32f4xx.h"
#include "adc.h"

#define SAMPLE_SIZE   20       // Number of readings for moving average
#define RISE_COUNT    5        // Consecutive rising samples to detect beat

static uint32_t readings[SAMPLE_SIZE] = {0};
static uint32_t sum = 0;
static uint8_t ptr = 0;

static uint32_t last_value = 0;
static uint32_t last_beat_time = 0;
static int32_t bpm = -1;

int32_t read_heart(uint32_t current_time_ms)
{
    // Read sensor and update moving average
    uint32_t raw = adc_read(0) * 500;
    sum -= readings[ptr];
    sum += raw;
    readings[ptr] = raw;
    ptr = (ptr + 1) % SAMPLE_SIZE;
    uint32_t avg = sum / SAMPLE_SIZE;

    // Detect rising slope
    static uint8_t rise_count = 0;
    if (avg > last_value)
        rise_count++;
    else
        rise_count = 0;

    // Beat detected when consecutive rises reach threshold
    if (rise_count >= RISE_COUNT)
    {
        if (current_time_ms - last_beat_time > 300) // minimum interval to avoid double-counting
        {
            uint32_t interval = current_time_ms - last_beat_time;
            bpm = 60000 / interval;   // Convert IBI to BPM
            last_beat_time = current_time_ms;
        }
        rise_count = 0; // reset for next beat
    }

    last_value = avg;

    return bpm; // Returns -1 if no new beat detected
}

