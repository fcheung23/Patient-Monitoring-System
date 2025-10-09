#include "stm32f4xx.h"
#include "i2c.h"

void I2C1_init(void)
{
	/* Pin out:
	 * PB8 --- SCL
	 * PB9 --- SDA
	 */
	// 1. Enable clock access to GPIOB
	RCC->AHB1ENR |= GPIOBEN;
	// 2. Set PB8 and PB9 mode to alternate function
	GPIOB->MODER &= ~(1U<<16);
	GPIOB->MODER |= (1U<<17);
	GPIOB->MODER &= ~(1U<<18);
	GPIOB->MODER |= (1U<<19);
	// AF4; see alternate function table
	GPIOB->AFR[1] &= ~(1U<<0);
	GPIOB->AFR[1] &= ~(1U<<1);
	GPIOB->AFR[1] |= (1U<<2);
	GPIOB->AFR[1] &= ~(1U<<3);
	GPIOB->AFR[1] &= ~(1U<<4);
	GPIOB->AFR[1] &= ~(1U<<5);
	GPIOB->AFR[1] |= (1U<<6);
	GPIOB->AFR[1] &= ~(1U<<7);
	// 3. Set PB8 and PB9 output type to open drain (prevent concurrent push-pull)
	GPIOB->OTYPER |= (1U<<8);
	GPIOB->OTYPER |= (1U<<9);
	// 4. Enable Pull-up for PB8 and PB9 (to pull default high)
	GPIOB->PUPDR |= (1U<<16);
	GPIOB->PUPDR &= ~(1U<<17);
	GPIOB->PUPDR |= (1U<<18);
	GPIOB->PUPDR &= ~(1U<<19);

	// 1. Enable clock access to I2C1
	RCC->APB1ENR |= I2C1EN;
	// 2. Enter reset mode
	I2C1->CR1 |= CR1_SWRST;
	// 3. Exit reset mode
	I2C1->CR1 &= ~(CR1_SWRST);
	// 4. Set peripheral clock frequency
	I2C1->CR2 = (1U<<4); // 16 MHz
	// 5. Set I2C to standard mode, 100 KHz
	I2C1->CCR = I2C_100KHZ;
	// 6. Set rise time
	I2C1->TRISE = SD_MODE_MAX_RISE_TIME;
	// 7. Enable I2C1 module
	I2C1->CR1 |= CR1_PE;
}


void I2C1_byteRead(char saddr, char maddr, char* data)
{
	volatile int tmp __attribute__((unused));

	// 1. Wait until bus not busy
	while(I2C1->SR2 & (SR2_BUSY)){}
	// 2. Generate start
	I2C1->CR1 |= CR1_START;
	// 3. Wait until start flag is set
	while(!(I2C1->SR1 & (SR1_SB))){}
	// 4. Transmit slave address +  0 (write) (7 bits ADDR + 1 bit R/W)
	I2C1->DR = saddr << 1;
	// 5. Wait until ADDR flag is set (received address matched)
	while(!(I2C1->SR1 & (SR1_ADDR))){}
	// 6. Clear ADDR flag
	tmp = I2C1->SR2;
	// 7. Send memory address
	I2C1->DR = maddr;
	// 8. Wait until transmitter (data register) empty
	while(!(I2C1->SR1 & (SR1_TXE))){}
	// 9. Generate restart
	I2C1->CR1 |= CR1_START;
	// 10. Wait until start flag is set
	while(!(I2C1->SR1 & (SR1_SB))){}
	// 11. Transmit slave address +  1 (read) (7 bits ADDR + 1 bit R/W)
	I2C1->DR = saddr << 1 | 1;
	// 12. Wait until ADDR flag is set (received address matched)
	while(!(I2C1->SR1 & (SR1_ADDR))){}
	// 13. Disable ACK
	I2C1->CR1 &= ~(CR1_ACK);
	// 14. Clear ADDR flag
	tmp = I2C1->SR2;
	// 15. Generate stop
	I2C1->CR1 |= CR1_STOP;
	// 16. Wait until RXNE flag is set
	while(!(I2C1->SR1 & (SR1_RXNE))){}
	// 17. Read data from DR
	*data++ = I2C1->DR;
}


void I2C1_burstRead(char saddr, char maddr, int n, char* data)
{
	volatile int tmp __attribute__((unused));

	// 1. Wait until bus not busy
	while(I2C1->SR2 & SR2_BUSY){}
	// 2. Generate start
	I2C1->CR1 |= CR1_START;
	// 3. Wait until start flag is set
	while(!(I2C1->SR1 & (SR1_SB))){}
	// 4. Transmit slave address +  0 (write) (7 bits ADDR + 1 bit R/W)
	I2C1->DR = saddr << 1;
	// 5. Wait until ADDR flag is set (received address matched)
	while(!(I2C1->SR1 & (SR1_ADDR))){}
	// 6. Clear ADDR flag
	tmp = I2C1->SR2;
	// 7. Wait until transmitter (data register) empty
	while(!(I2C1->SR1 & (SR1_TXE))){}
	// 8. Send memory address to read from
	I2C1->DR = maddr;
	// 9. Wait until transmitter (data register) empty
	while(!(I2C1->SR1 & (SR1_TXE))){}
	// 10. Generate restart
	I2C1->CR1 |= CR1_START;
	// 11. Wait until start flag is set
	while(!(I2C1->SR1 & (SR1_SB))){}
	// 12. Transmit slave address +  1 (read) (7 bits ADDR + 1 bit R/W)
	I2C1->DR = saddr << 1 | 1;
	// 13. Wait until ADDR flag is set (received address matched)
	while(!(I2C1->SR1 & (SR1_ADDR))){}
	// 14. Clear ADDR flag
	tmp = I2C1->SR2;
	// 15. Enable ACK
	I2C1->CR1 |= CR1_ACK;
	while(n > 0U)
	{
		// If one byte is left
		if(n == 1U)
		{
			// Disable ACK
			I2C1->CR1 &= ~(CR1_ACK);
			// Generate stop
			I2C1->CR1 |= CR1_STOP;
			// Wait until RXNE flag is set
			while(!(I2C1->SR1 & (SR1_RXNE))){}
			// Read data from DR
			*data++ = I2C1->DR;
			// Done
			break;
		}
		else
		{
			// Wait until RXNE flag is set
			while(!(I2C1->SR1 & (SR1_RXNE))){}
			// Read data from DR
			(*data++) = I2C1->DR;
			// Keep going
			n--;
		}
	}
}


void I2C1_burstWrite(char saddr, char maddr, int n, char* data)
{
	volatile int tmp __attribute__((unused));

	// 1. Wait until bus not busy
	while(I2C1->SR2 & SR2_BUSY){}
	// 2. Generate start
	I2C1->CR1 |= CR1_START;
	// 3. Wait until start flag is set
	while(!(I2C1->SR1 & (SR1_SB))){}
	// 4. Transmit slave address +  0 (write) (7 bits ADDR + 1 bit R/W)
	I2C1->DR = saddr << 1;
	// 5. Wait until ADDR flag is set (received address matched)
	while(!(I2C1->SR1 & (SR1_ADDR))){}
	// 6. Clear ADDR flag
	tmp = I2C1->SR2;
	// 7. Wait until transmitter (data register) empty
	while(!(I2C1->SR1 & (SR1_TXE))){}
	// 8. Send memory address to write to
	I2C1->DR = maddr;
	// 9. Write all bytes to I2C
	for (int i = 0; i < n; i++)
	{
		// Wait until transmitter (data register) empty
		while(!(I2C1->SR1 & (SR1_TXE))){}
		// Transmit memory address (data to write)
		I2C1->DR = *data++;
	}
	// 10. Wait until transfer finished
	while(!(I2C1->SR1 & (SR1_BTF))){}
	// 11. Generate stop
	I2C1->CR1 |= CR1_STOP;
}
