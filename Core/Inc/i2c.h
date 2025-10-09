/*
 * i2c.h
 *
 *  Created on: Jul 22, 2025
 *      Author: fcheung
 */

#ifndef I2C_H_
#define I2C_H_

#define GPIOBEN			(1U<<1)
#define I2C1EN			(1U<<21)

#define CR1_SWRST		(1U<<15)
#define CR1_PE			(1U<<0)
#define I2C_100KHZ				80		// 0B 0101 000 = Decimal = 50
#define SD_MODE_MAX_RISE_TIME	17

#define SR2_BUSY		(1U<<1)
#define CR1_START		(1U<<8)
#define SR1_SB			(1U<<0)
#define SR1_ADDR		(1U<<1)
#define SR1_TXE			(1U<<7)
#define CR1_ACK			(1U<<10)
#define CR1_STOP		(1U<<9)
#define SR1_RXNE		(1U<<6)
#define SR1_BTF			(1U<<2)

/**
 * Configures PB8 (SCL) and PB9 (SDA) for I2C1
 *
 * @param	None
 * @returns	None
 */
void I2C1_init(void);


/**
 * I2C reads byte into data_rec[] buffer
 *
 * @param	char slave address, char master address, char* data storage location
 * @returns	None
 */
void I2C1_byteRead(char saddr, char maddr, char* data);


/**
 * I2C reads bytes into data_rec[] buffer
 *
 * @param	char slave address, char master address, int number of bytes, char* data storage location
 * @returns	None
 */
void I2C1_burstRead(char saddr, char maddr, int n, char* data);


/**
 * I2C writes values to device
 *
 * @param	char slave address, char master address, int number of bytes, char* data to write location
 * @returns	None
 */
void I2C1_burstWrite(char saddr, char maddr, int n, char* data);

#endif /* I2C_H_ */
