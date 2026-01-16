#include "main.h"
#include "cmsis_os.h"
#include "stdio.h"
#include <stdlib.h>
#include "data_packet.h"

// Sensors
#include "tmp36.h"
#include "adxl345.h"
#include "ky039.h"

UART_HandleTypeDef huart2;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);


// Initialize data packets with sensor source
static Data_t xStructToSend[3] =
{
    { .sDataSource = temperature_sensor },
    { .sDataSource = heartbeat_sensor },
    { .sDataSource = accelerometer_sensor }
};

TaskHandle_t receiver_handle, temp_task_handle, heart_task_handle, accel_task_handle;

QueueHandle_t xTempQueue;
QueueHandle_t xHeartQueue;
QueueHandle_t xAccelQueue;

QueueSetHandle_t xQueueSet;

void queue_setup();
void ReceiverTask(void *pvParameters);
void SenderTask(void *pvParameters);

int main(void)
{
	HAL_Init();
	SystemClock_Config();
	MX_GPIO_Init();
	MX_USART2_UART_Init();

	I2C1_init();
	ADC1_DMA2_init();

	adxl_init();		// adxl345
	start_conversion();	// tmp36 + ky039

	xTaskCreate(ReceiverTask, "Receiver Task", 100, NULL, 1, &receiver_handle);

	// Pass in pointer to structs with just the sDataSource info
	xTaskCreate(SenderTask, "Temperature Sender Task", 100, (void *)&(xStructToSend[0]),
	2, &temp_task_handle);

	xTaskCreate(SenderTask, "Heartbeat Sender Task", 100, (void *)&(xStructToSend[1]),
	2, &heart_task_handle);

	xTaskCreate(SenderTask, "Accelerometer Sender Task", 100, (void *)&(xStructToSend[2]),
	2, &accel_task_handle);

	vTaskStartScheduler();

	while (1) {}

}

void queue_setup()
{
	// Create queues per sensor
	xTempQueue = xQueueCreate(1, sizeof(Data_t));
	xHeartQueue = xQueueCreate(1, sizeof(Data_t));
	xAccelQueue = xQueueCreate(1, sizeof(Data_t));

	xQueueSet = xQueueCreateSet(3 * 1); // # queues x # length per queue

	xQueueAddToSet(xTempQueue, xQueueSet);
	xQueueAddToSet(xHeartQueue, xQueueSet);
	xQueueAddToSet(xAccelQueue, xQueueSet);
}

int uart2_write(int ch)
{
	// Check that TXE is empty
	while(!(USART2->SR & USART_SR_TXE)){}

	// Write to TXE
	USART2->DR = (ch & 0xFF);
	return ch;
}

int __io_putchar(int ch)
{
	uart2_write(ch);
	return ch;
}

void SenderTask(void *pvParameters)
{
	BaseType_t qState;
	const TickType_t wait_time = pdMS_TO_TICKS(200);
	Data_t *pPacket = pvParameters;
	queue_setup();

	while(1)
	{
		// Populates values and sends to corresponding queue based on which sensor task uses function
		switch(pPacket->sDataSource)
		{
			case temperature_sensor:
				pPacket->payload.temp.tempValue = read_tmp();
				qState = xQueueOverwrite(xTempQueue, pPacket);
				break;
			case heartbeat_sensor:
				int possible_bpm = read_heart();  // Make sure new reading is available
				if (possible_bpm != -1) pPacket->payload.heart.bpmValue = possible_bpm;
				qState = xQueueOverwrite(xHeartQueue, pPacket);
				break;
			case accelerometer_sensor:
				pPacket->payload.accel.vectorMag = read_accel();
				qState = xQueueOverwrite(xAccelQueue, pPacket);
				break;
			default:
				printf("Error: Invalid sensor source.");
				break;
		}

		if(qState != pdPASS)
		{
			printf("Error: Send failed for %d\r\n", pPacket->sDataSource);
		}

		vTaskDelay(pdMS_TO_TICKS(10));
	}
}

void ReceiverTask(void *pvParameters)
{
    QueueSetMemberHandle_t xActivatedQueue;
    Data_t rxData;

    while (1)
    {
        // Block until any queue in the set has data ready
        xActivatedQueue = xQueueSelectFromSet(xQueueSet, portMAX_DELAY);

        if (xActivatedQueue == xTempQueue)
        {
            xQueueReceive(xTempQueue, &rxData, portMAX_DELAY);
            int32_t tempC_x10 = rxData.payload.temp.tempValue;
			int32_t tempC_whole = tempC_x10 / 10;
			int32_t tempC_tenths = abs(tempC_x10 % 10);
			printf("Temperature: %ld.%ld C\n\r", tempC_whole, tempC_tenths);
        }
        else if (xActivatedQueue == xHeartQueue)
        {
            xQueueReceive(xHeartQueue, &rxData, portMAX_DELAY);
            int32_t bpm = rxData.payload.heart.bpmValue;
            if (bpm > 100 || bpm < 60) printf("ALERT! | Abnormal Heartbeat!");
            printf("Heartbeat = %ld BPM\r\n", bpm);
        }
        else if (xActivatedQueue == xAccelQueue)
        {
            xQueueReceive(xAccelQueue, &rxData, portMAX_DELAY);
            int32_t vectorMag_x10 = rxData.payload.accel.vectorMag;
            int32_t vectorMag_whole = vectorMag_x10 / 100;
            int32_t vectorMag_decimal = abs(vectorMag_x10 % 100);
            printf("Movement (vector magnitude): %ld.%ld m/s^2\n\r", vectorMag_whole, vectorMag_decimal);
        }
    }
}




// <----------------------------------------------- GENERATED | DO NOT EDIT --------------------------------------------->

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */


/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM11 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM11) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

