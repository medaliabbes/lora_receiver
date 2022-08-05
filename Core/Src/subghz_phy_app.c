/*!
 * \file      subghz_phy_app.c
 *
 * \brief     Ping-Pong implementation
 *
 * \copyright Revised BSD License, see section \ref LICENSE.
 *
 * \code
 *                ______                              _
 *               / _____)             _              | |
 *              ( (____  _____ ____ _| |_ _____  ____| |__
 *               \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 *               _____) ) ____| | | || |_| ____( (___| | | |
 *              (______/|_____)_|_|_| \__)_____)\____)_| |_|
 *              (C)2013-2017 Semtech
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Gregory Cristian ( Semtech )
 */
/**
  ******************************************************************************
  *
  *          Portions COPYRIGHT 2020 STMicroelectronics
  *
  * @file    subghz_phy_app.c
  * @author  MCD Application Team
  * @brief   Application of the SubGHz_Phy Middleware
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "platform.h"
//#include "sys_app.h"
#include "subghz_phy_app.h"
#include "radio.h"
#include "app_version.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
/* USER CODE BEGIN Includes */
/*
#include "stm32_timer.h"
#include "stm32_seq.h"
#include "utilities_def.h"
*/
/* USER CODE END Includes */

/* External variables ---------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private typedef -----------------------------------------------------------*/

/* USER CODE BEGIN PTD */
typedef enum
{
  RX,
  RX_TIMEOUT,
  RX_ERROR,
  TX,
  TX_TIMEOUT,
} States_t;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* Configurations */
/*Timeout*/
#define RX_TIMEOUT_VALUE              3000
#define TX_TIMEOUT_VALUE              3000
/* PING string*/
#define PING "PING"
/* PONG string*/
#define PONG "PONG"
/*Size of the payload to be sent*/
/* Size must be greater of equal the PING and PONG*/
#define MAX_APP_BUFFER_SIZE          255
#if (PAYLOAD_LEN > MAX_APP_BUFFER_SIZE)
#error PAYLOAD_LEN must be less or equal than MAX_APP_BUFFER_SIZE
#endif /* (PAYLOAD_LEN > MAX_APP_BUFFER_SIZE) */
/* wait for remote to be in Rx, before sending a Tx frame*/
#define RX_TIME_MARGIN                200
/* Afc bandwidth in Hz */
#define FSK_AFC_BANDWIDTH             83333
/* LED blink Period*/
#define LED_PERIOD_MS                 200
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* Radio events function pointer */
static RadioEvents_t RadioEvents;
/* USER CODE BEGIN PV */

/*Ping Pong FSM states */
static States_t State = RX;
/* App Rx Buffer*/
static uint8_t BufferRx[MAX_APP_BUFFER_SIZE];
/* App Tx Buffer*/
static uint8_t BufferTx[MAX_APP_BUFFER_SIZE];
/* Last  Received Buffer Size*/
uint16_t RxBufferSize = 0;
/* Last  Received packer Rssi*/
int8_t RssiValue = 0;
/* Last  Received packer SNR (in Lora modulation)*/
int8_t SnrValue = 0;
/* Led Timers objects*/

/* device state. Master: true, Slave: false*/
bool isMaster = true;
/* random delay to make sure 2 devices will sync*/
/* the closest the random delays are, the longer it will
   take for the devices to sync when started simultaneously*/
static int32_t random_delay;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/*!
 * @brief Function to be executed on Radio Tx Done event
 */
static void OnTxDone(void);

/**
  * @brief Function to be executed on Radio Rx Done event
  * @param  payload ptr of buffer received
  * @param  size buffer size
  * @param  rssi
  * @param  LoraSnr_FskCfo
  */
static void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t LoraSnr_FskCfo);

/**
  * @brief Function executed on Radio Tx Timeout event
  */
static void OnTxTimeout(void);

/**
  * @brief Function executed on Radio Rx Timeout event
  */
static void OnRxTimeout(void);

/**
  * @brief Function executed on Radio Rx Error event
  */
static void OnRxError(void);

/* USER CODE BEGIN PFP */
/**
  * @brief  Function executed on when led timer elapses
  * @param  context ptr of LED context
  */


/**
  * @brief PingPong state machine implementation
  */

/* USER CODE END PFP */

/* Exported functions ---------------------------------------------------------*/
void SubghzApp_Init(void)
{
  /* USER CODE BEGIN SubghzApp_Init_1 */
  printf( "\n\rPING PONG\n\r");
  /* Print APP version*/
  printf( "APP_VERSION= V%X.%X.%X\r\n",
          (uint8_t)(__APP_VERSION >> __APP_VERSION_MAIN_SHIFT),
          (uint8_t)(__APP_VERSION >> __APP_VERSION_SUB1_SHIFT),
          (uint8_t)(__APP_VERSION >> __APP_VERSION_SUB2_SHIFT));

  /* Radio initialization */
  RadioEvents.TxDone = OnTxDone;
  RadioEvents.RxDone = OnRxDone;
  RadioEvents.TxTimeout = OnTxTimeout;
  RadioEvents.RxTimeout = OnRxTimeout;
  RadioEvents.RxError = OnRxError;

  Radio.Init(&RadioEvents);

  /* USER CODE BEGIN SubghzApp_Init_2 */
  /* Radio Set frequency */
  Radio.SetChannel(RF_FREQUENCY);

  /* Radio configuration */
  printf( "---------------\n\r");
  printf("LORA_MODULATION\n\r");
  printf( "LORA_BW=%d kHz\n\r", (1 << LORA_BANDWIDTH) * 125);
  printf( "LORA_SF=%d\n\r", LORA_SPREADING_FACTOR);

  Radio.SetTxConfig(MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                    LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                    LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                    true, 0, 0, LORA_IQ_INVERSION_ON, TX_TIMEOUT_VALUE);

  Radio.SetRxConfig(MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                    LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                    LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                    0, true, 0, 0, LORA_IQ_INVERSION_ON, true);

  Radio.SetMaxPayloadLength(MODEM_LORA, MAX_APP_BUFFER_SIZE);

  /* LED initialization*/
 ;
  /*calculate random delay for synchronization*/
  random_delay = (Radio.Random()) >> 22; /*10bits random e.g. from 0 to 1023 ms*/
  /*fills tx buffer*/
  memset(BufferTx, 0x0, MAX_APP_BUFFER_SIZE);

  printf( "rand=%ld\n\r", random_delay);
  /*starts reception*/
  Radio.Rx(RX_TIMEOUT_VALUE + random_delay);

}

/* USER CODE BEGIN EF */

/* USER CODE END EF */

/* Private functions ---------------------------------------------------------*/

static void OnTxDone(void)
{
  /* USER CODE BEGIN OnTxDone */
  printf( "OnTxDone\n\r");
  /* Update the State of the FSM*/
  State = TX;
}

#include "packet.h"

static void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t LoraSnr_FskCfo)
{
    /* USER CODE BEGIN OnRxDone */
	printf( "OnRxDone\n\r");
	char * recv = (char*) malloc(sizeof(char) * (size +1));
	memcpy(recv , payload , size) ;
	recv[size] = '\0' ;
	printf( "recv : %s\n\r" , recv);

	free(recv) ;
	packet_t p ;
	if(packet_desirialize(payload , size , &p) ==PACK_OK)
	{
		debug_packet(&p) ;
	}
	else{
		printf("packet corrupt\n");
	}
	printf( "RssiValue=%d dBm, SnrValue=%ddB\n\r", rssi, LoraSnr_FskCfo);
    /* Record payload Signal to noise ratio in Lora*/
    SnrValue = LoraSnr_FskCfo;

    /* Update the State of the FSM*/
    State = RX;
    /* Clear BufferRx*/
	memset(BufferRx, 0, MAX_APP_BUFFER_SIZE);
	/* Record payload size*/
	RxBufferSize = size;
	if (RxBufferSize <= MAX_APP_BUFFER_SIZE)
	{
		memcpy(BufferRx, payload, RxBufferSize);
	}
	/* Record Received Signal Strength*/
	RssiValue = rssi;
	/* Record payload content*/

}

static void OnTxTimeout(void)
{
  /* USER CODE BEGIN OnTxTimeout */
  printf( "OnTxTimeout\n\r");
  /* Update the State of the FSM*/
  State = TX_TIMEOUT;
}

static void OnRxTimeout(void)
{
  /* USER CODE BEGIN OnRxTimeout */
  printf("OnRxTimeout\n\r");
  /* Update the State of the FSM*/
  State = RX_TIMEOUT;
  /* Run PingPong process in background*/
 // UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_SubGHz_Phy_App_Process), CFG_SEQ_Prio_0);
  /* USER CODE END OnRxTimeout */
}

static void OnRxError(void)
{
  /* USER CODE BEGIN OnRxError */
  printf( "OnRxError\n\r");
  /* Update the State of the FSM*/
  State = RX_ERROR;
}


const char * str = "stm32 sender";
uint32_t tmr = 0 ;

#define STATE_LISTEN    0
#define STATE_TRANSMIT  1


uint8_t state  = STATE_LISTEN ;


void PingPong_Process(void)
{

  static uint8_t send_counter = 0   ;
  //static uint8_t recv_counter = 0 ;

  Radio.Sleep();

  switch(state)
  {
  	  case STATE_LISTEN :
  		  Radio.Rx(2500) ;

  		  printf( "enter recv state\n\r");

  		  HAL_Delay(2500);
  		  state = STATE_TRANSMIT ;
  		  break ;
  	  case STATE_TRANSMIT :

  		  Radio.Send((uint8_t*)str,strlen(str)) ;
  		  HAL_Delay(200);
  		  //printf( "tran state\n\r");
  		  send_counter++ ;

  		  if(send_counter >= 10)
  		  {
  			  state = STATE_LISTEN ;
  			  send_counter = 0;
  		  }
  		  break ;
  	  default :
  		  printf( "error state\n\r");
  		  break ;
  }

}





/* USER CODE END PrFD */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
