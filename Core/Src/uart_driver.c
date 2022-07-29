

#include "uart_driver.h"
#include "ring.h"


uint8_t uart_rxbuffer[UART_BUFFER_LENGTH] ;
Ring_t uart_fifo ;
extern UART_HandleTypeDef huart1 ;

bool NL_Detection = false ;


void set_NL_detection()
{
	NL_Detection = true ;
}

void clear_NL_detection()
{
	NL_Detection = false ;
}

bool get_NL_detection()
{
	return NL_Detection ;
}


void uart_driver_init()
{
	// here can add hardware initialisation and enable interrupt
	uart_fifo = ring_init(uart_rxbuffer , UART_BUFFER_LENGTH) ;
	//enable uart Rx-interrupt
	__HAL_UART_ENABLE_IT(&huart1 , UART_IT_RXNE);
}

void uart_RX_interrupt_hanlder(uint8_t rx)
{
	if(rx == '\n')
		set_NL_detection() ;

	ring_pushOver(&uart_fifo , rx) ;
}

/**
 * read from uart_fifo until '\n'
 */
int uart_read_line(char * data)
{

	if(get_NL_detection() == false)
		return 0;

	int len = ring_popAvailable(uart_fifo)  ;

    if(len == 0 )
    	return 0;

    int i = 0 ;

    for( ; i< len ; i++)
    {
    	ring_pop(&uart_fifo , (uint8_t*)&data[i] ) ;
    	if(data[i] == '\n')
    	{
    		clear_NL_detection() ;
    		break ;
    	}
    }

    return i++ ;

}
