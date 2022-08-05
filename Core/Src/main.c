



#include "main.h"
#include "stdio.h"
#include "stdlib.h"
#include "radio.h"
#include "subghz_phy_app.h"
#include "uart_driver.h"
#include "config.h"
#include "string.h"

#include "proto_types.h"
#include "ll.h"


#define CONFIG_PASSWORD        "root123"
#define STATE_IDLE 				0x00  //
#define STATE_WAIT_PASSWORD		0x01  //
#define STATE_PASS_ACCEPTED		0x02
#define STATE_GET_CONFIG		0x03
#define STATE_SAVE	    		0x04

extern void sys_delay(u32 x)
{
	HAL_Delay(x);
}

UART_HandleTypeDef huart1;

extern const struct Radio_s Radio;

static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
void SystemClock_Config(void) ;
void config_debit_seuil(char * input , int input_len) ;


config_t config_param ;
config_t config_param_copy ; // this should be loaded from the flash

int main(void)
{

  HAL_Init();

  SystemClock_Config();

  MX_GPIO_Init();
  MX_USART1_UART_Init();
  SubghzApp_Init();

  //Rx pin should not be floating
  //uart_driver_init() ;

  //Radio.Rx(1000);

  printf("Yes we did 'it!!\n");
  printf("enter your config\n");

  /************load configuration from flash and print it ************/
  config_init();

  config_load(&config_param) ;

  memcpy(&config_param_copy , &config_param , sizeof(config_t));

  printf("param seuil %f , debit %f\n",config_param.seuil , config_param.debit) ;

  /********************************************************************/
  //Radio.Rx(1000);
  ll_init(52) ;

   ll_send_to(77 , (u8*)"medali" , 6);

  ll_send_to(77 , (u8*)"abbes" , 5);

  while (1)
  {
	  /* USER CODE END WHILE */

	  //PingPong_Process() ;

	  HAL_Delay(1000);

	  ll_transmit();


	  //int len = uart_read_line(str) ;

	  //config_debit_seuil(str , len) ;

  }
  /* USER CODE END 3 */
}




void config_debit_seuil(char * input , int input_len)
{
	static unsigned char config_state = STATE_IDLE ;
	//printf("con debit :%s ,%d\n" ,input , input_len) ;
	input[input_len] = '\0' ;
	switch(config_state)
	{
	case STATE_IDLE :
		if(strncmp("config" ,input , 6)==0 && input_len -1 == 6)
		{
			//make transition here
			config_state = STATE_WAIT_PASSWORD ;
			printf("enter password\n") ;
		}
		break ;

	case STATE_WAIT_PASSWORD :

		if(strncmp(CONFIG_PASSWORD , input , strlen(CONFIG_PASSWORD)) == 0
				&& input_len - 1 == strlen(CONFIG_PASSWORD))
		{
			config_state = STATE_GET_CONFIG ;
			printf("enter your config\n");
		}
		break ;

	case STATE_GET_CONFIG :

		if(strncmp("seuil:" , input ,6) == 0 && input_len >= 6)
		{
			printf("%s\n" , input) ;
			char seu[10] ;
			strncpy(seu , &input[6] , input_len -7) ;
			config_param_copy.seuil = atof(seu) ;
			(void)seu ;
		}
		else if(strncmp("debit:" , input ,6) == 0 && input_len >= 6)
		{
			printf("%s\n" , input) ;
			char deb[10] ;
			strncpy(deb , &input[6] ,input_len -7 ) ;

			//config_param_copy
			config_param_copy.debit = atof(deb) ;
			(void) deb ;
		}
		else if(strncmp("save" , input ,4 ) == 0 && input_len -1 == 4)
		{
			config_state = STATE_SAVE ;
			memcpy(&config_param ,&config_param_copy , sizeof(config_param));
		}
		else if(strncmp("discard" , input ,7) == 0 && input_len -1 == 7)
		{
			//can reload the struct config from the flash
			memcpy(&config_param_copy , &config_param , sizeof(config_t)) ;
			config_state = STATE_IDLE ;
			printf("idle\n");
		}
		break ;

	case STATE_SAVE :
		//save to the flash
		printf("saved\n");
		config_save(&config_param_copy) ;
		config_state = STATE_IDLE ;
		printf("NEW CONFIGURATION : seuil :%f ,debit : %f \n"
				,config_param.seuil ,config_param.debit) ;
		break ;

	default :
		break ;
	}
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);
  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE|RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_11;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure the SYSCLKSource, HCLK, PCLK1 and PCLK2 clocks dividers
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK3|RCC_CLOCKTYPE_HCLK
                              |RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1
                              |RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.AHBCLK3Divider = RCC_SYSCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}



/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */
  __NVIC_EnableIRQ(USART1_IRQn);
  __NVIC_SetPriority(USART1_IRQn , 0x1);
  /* USER CODE END USART1_Init 2 */
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

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

int __io_getchar(void)
{
	int ch ;
	HAL_UART_Receive(&huart1 , (uint8_t*)&ch , 1 ,0xffff);
	return ch ;
}

int __io_putchar(int ch)
{
	HAL_UART_Transmit(&huart1 , (uint8_t*) &ch,1,10);
	return ch ;
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
