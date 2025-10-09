#include "stm32f4xx.h"
#include "adxl345.h"
#include "math.h"

char data;
uint8_t data_rec[6];

void adxl_read_address(uint8_t reg)
{
	I2C1_byteRead(DEVICE_ADDR, reg, &data);
}


void adxl_write(uint8_t reg, char value)
{
	char data[1];
	data[0] = value;

	I2C1_burstWrite(DEVICE_ADDR, reg, 1, data);
}


void adxl_read_values(uint8_t reg)
{
	I2C1_burstRead(DEVICE_ADDR, reg, 6, (char *)data_rec);
}


void adxl_init(void)
{
	// 1. I2C enabled in setup
	// 2. Read the DEVID, this should return 0xE5
	adxl_read_address(DEVID_R);
	// 3. Set data format range to +-4g
	adxl_write(DATA_FORMAT_R, FOUR_G);
	// 4. Reset all bits
	adxl_write(POWER_CTL_R, RESET);
	// 5. Configure power control measure bit
	adxl_write(POWER_CTL_R, SET_MEASURE_B);
}


int32_t read_accel(void)
{
	adxl_read_values(DATA_START_ADDR);

	int16_t x = ((data_rec[1]<<8)|data_rec[0]);
	int16_t y = ((data_rec[3]<<8)|data_rec[2]);
	int16_t z = ((data_rec[5]<<8)|data_rec[4]);

	// 0.0078 for 4g, 0.893 for measured rest calibration
	float xg = (x * 0.0078) * 0.893;
	float yg = (y * 0.0078) * 0.893;
	float zg = (z * 0.0078) * 0.893;

	float vectorMag_x10 = sqrtf(xg*xg + yg*yg + zg*zg) * 100; // * 100 for decimal precision

	return (int32_t) vectorMag_x10;
}

