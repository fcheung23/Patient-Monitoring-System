/*
 * adc.h
 *
 *  Created on: Jul 12, 2025
 *      Author: fcheung
 */

#ifndef ADC_H_
#define ADC_H_

#define ADC1EN     (1U<<8)
#define GPIOAEN    (1U<<0)
#define DMA2EN     (1U<<22)

#define CR1_SCAN   (1U<<8)
#define CR2_CONT   (1U<<1)
#define CR2_ADON   (1U<<0)
#define CR2_DMA    (1U<<8)
#define CR2_DDS    (1U<<9)
#define ADC_SEQ_LEN_2 (1U<<20)
#define ADC_CH0    0
#define ADC_CH1    1

#define CR_EN	  (1U<<0)
#define CR_MINC	  (1U<<10)
#define CR_CIRC	  (1U<<8)


/**
 * Configures PA0 and PA1 for multi-channel ADC1 conversion, uses DMA buffer adc_data[]
 *
 * @param	None
 * @returns	None
 */
void ADC1_DMA2_init(void);


/**
 * Starts ADC1 conversion
 *
 * @param	None
 * @returns	None
 */
void start_conversion(void);


/**
 * Returns ADC1 conversion value from corresponding channel in DMA buffer
 *
 * @param	uint8_t channel number, either 0 or 1
 * @returns	uint16_t raw conversion value
 */
uint16_t adc_read(uint8_t channel);


#endif /* ADC_H_ */
