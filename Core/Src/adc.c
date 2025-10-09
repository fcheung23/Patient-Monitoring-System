#include "stm32f4xx.h"
#include <stdint.h>
#include "adc.h"

uint16_t adc_data[2]; // DMA buffer

void ADC1_DMA2_init(void)
{
    // 1. Enable clock access to GPIOA, DMA2, ADC1
    RCC->AHB1ENR |= GPIOAEN | DMA2EN;
    RCC->APB2ENR |= ADC1EN;
    // 2. Set PA0 and PA1 mode to analog
    GPIOA->MODER |= (3U << (0 * 2)) | (3U << (1 * 2));

    // 3. Disable DMA2 Stream0
    DMA2_Stream0->CR &= ~CR_EN;
    while (DMA2_Stream0->CR & CR_EN); // wait

    // 4. Set DMA peripheral and memory addresses
    DMA2_Stream0->PAR  = (uint32_t) &ADC1->DR;
    DMA2_Stream0->M0AR = (uint32_t) adc_data;
    // 5. Set number of data items
    DMA2_Stream0->NDTR = 2;
    // 6. Clear DMA CR
    DMA2_Stream0->CR = 0;
    // 7. Configure DMA channel to 0 for ADC1
    DMA2_Stream0->CR |= (0U<<25);
    // 8. Enable memory increment mode
    DMA2_Stream0->CR |= CR_MINC;
    // 9. Set memory size to half-word 16-bit
    DMA2_Stream0->CR |= (1U<<13);
    // 10. Set peripheral size to 16-bit
    DMA2_Stream0->CR |= (1U<<11);
    // 11. Enable circular mode
	DMA2_Stream0->CR |= CR_CIRC;
    // 12. Set data transfer direction to peripheral-to-memory
    DMA2_Stream0->CR &= ~(3U << 6);

    // 13. Enable ADC scan mode
    ADC1->CR1 = CR1_SCAN;
    // 14. Set sequence length to 2 conversions
    ADC1->SQR1 = ADC_SEQ_LEN_2;
    // 15. Set sequence registers for channels 0 and 1
    ADC1->SQR3 = (ADC_CH0) | (ADC_CH1 << 5);
    // 16. Configure ADC for continuous mode, DMA, and DDS
    ADC1->CR2 = CR2_CONT | CR2_DMA | CR2_DDS;

    // 17. Enable ADC
    ADC1->CR2 |= CR2_ADON;
    // 18. Enable DMA stream
    DMA2_Stream0->CR |= CR_EN;
}


void start_conversion(void)
{
    ADC1->CR2 |= (1U << 30);
}


uint16_t adc_read(uint8_t channel)
{
    if (channel > 1) return 0;
    return adc_data[channel];
}

