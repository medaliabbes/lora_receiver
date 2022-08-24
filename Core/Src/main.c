
/*
 * Thursday 19/08/2022
 * added config_save1 , store_nodes1
 *
 * those to function does not erase data when they are called after each other
 * they are tested .
 *
 */


/**
 * callback added
 * Rx buffer full solved
 */

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


#define TRANSMITTER_ADDRESS    (52)
#define RECEIVER_ADDRESS 	   (77)

#define DEFAULT_FEEDBACK_PERIOD		(10*60)  //10 min

UART_HandleTypeDef huart1;

extern const struct Radio_s Radio;

static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
void SystemClock_Config(void) ;



int parse_transmetter_data(char * t_data , int len , float *seuil , int *periode) ;

float get_flow(int periode)  ;

void open_vanne()  ;

void close_vanne() ;

int number_of_pulses  = 0 ;

void callBack()
{
	printf("Ask CallBack reset counter\n");
	number_of_pulses = 0 ;
}


int main(void)
{

  int feedback_periode ;

  HAL_Init();

  SystemClock_Config();

  MX_USART1_UART_Init();

  SubghzApp_Init();

  MX_GPIO_Init();

  config_t param ;

  config_init();

  int sta = config_load(&param) ;

  printf("Node receiver \n");

  printf("saved param seuil %f , periode %d\n",param.seuil , param.periode) ;

  if(sta == CONFIG_OK)
  {
	  printf("Valid data\n");

	  feedback_periode = param.periode ;
  }
  else{

	  printf("Invalid data\n");

	  feedback_periode = DEFAULT_FEEDBACK_PERIOD ;

  }

  ll_init(RECEIVER_ADDRESS) ;

  char feedback[50];

  u8 recv[50] ;

  //to send data whene started
  uint32_t feedback_timer = HAL_GetTick() + (feedback_periode *1000)+ 1 ;

  uint32_t tx_monitor = HAL_GetTick() ;

  int len = 0;

  while (1)
  {
	  int src = ll_get_recv(recv , &len) ;

	  if(len>0)
	  {
		  recv[len] = 0 ;

		  printf("data from %d: %s$\n" , src ,recv) ;

		  //Valide transmitter frame
		  if( parse_transmetter_data((char*)recv , len ,&param.seuil,&param.periode ) == 0)
		  {
			  printf("config seuil :%0.2f, per :%d\n",param.seuil , param.periode);
			  //save to the flash
			  config_save1(&param) ;

			  feedback_periode = param.periode ;

			  feedback_timer =  HAL_GetTick() ;
		  }
		  else
		  {
			  //nope()
		  }

	  }

	  //sending feedback every feedback_periode
	  if(HAL_GetTick() - feedback_timer >= feedback_periode * 1000)
	  {
		  float flow = get_flow(feedback_periode /1000) ;
		  sprintf(feedback ,"seuil :%f" ,flow);
		  ll_send_to(TRANSMITTER_ADDRESS ,(u8*) feedback , strlen(feedback) ,&callBack );
		  printf("nb pulse %d ,debit %f ,periode %d\n" ,number_of_pulses ,flow ,feedback_periode) ;
		  feedback_timer = HAL_GetTick() ;
	  }

	  if(HAL_GetTick() - tx_monitor >= 5000 )
	  {
		printf("tx buffer size :%d , rx buffer size :%d\n",get_tx_size(),get_rx_size() );
		tx_monitor = HAL_GetTick() ;
	  }

	  if(get_flow(feedback_periode /1000) >= param.seuil)
	  {
		  close_vanne() ;
	  }
	  else
	  {

	  }

	  ll_process() ;
  }
  /* USER CODE END 3 */
}


int parse_transmetter_data(char * t_data , int len , float *seuil , int *periode)
{
	char tmp[6] ;

	//seuil :%0.2f , periode : %d
	char * per = strstr(t_data , ", periode :");

	if(per == NULL)
	{
		return -1 ;
	}

	char * se = strstr(t_data , "seuil :");

	if(se == NULL)
	{
		return -1;
	}

	se += 7 ;
	memcpy(tmp , se, per - se);
	*seuil = atof(tmp) ;

	per += 12;
	memcpy(tmp , per ,(t_data+len - 1) - per) ;

	*periode = atoi(per) ;

	return 0 ;
}

//debit
float get_flow(int periode)
{
	return (float) (number_of_pulses / periode) ;
}

void open_vanne()
{

}

void close_vanne()
{

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


  GPIO_InitTypeDef pin ;

  EXTI_ConfigTypeDef exti ;
  EXTI_HandleTypeDef exti_handler ;

  exti_handler.Line = EXTI_LINE_0 ;

  exti.Line    =  EXTI_LINE_0;
  exti.Mode    =  EXTI_MODE_INTERRUPT;
  exti.Trigger =  EXTI_TRIGGER_RISING;
  exti.GPIOSel =  EXTI_GPIOB;


  HAL_EXTI_SetConfigLine(&exti_handler , &exti ) ;

  pin.Mode  = GPIO_MODE_IT_RISING ;
  pin.Pin   = GPIO_PIN_0  ;
  pin.Pull  = GPIO_PULLDOWN  ;
  pin.Speed = GPIO_SPEED_FREQ_HIGH ;

  HAL_GPIO_Init(GPIOB , &pin ) ;

  __NVIC_EnableIRQ(EXTI0_IRQn);
  __NVIC_SetPriority(EXTI0_IRQn , 0x0);

}


void EXTI0_IRQHandler(void)
{
	number_of_pulses++;
	printf("int\n");
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0) ;

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

extern void sys_delay(u32 x)
{
	HAL_Delay(x);
}

extern u32  sys_get_tick()
{
	return HAL_GetTick() ;
}

extern u8   sys_random()
{
	return get_random() % 255 ;
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
