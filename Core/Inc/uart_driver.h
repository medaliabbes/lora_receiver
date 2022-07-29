
#include "stdbool.h"
#include "main.h"

#define UART_BUFFER_LENGTH    100

void set_NL_detection();

void clear_NL_detection();

bool get_NL_detection();

void uart_driver_init(void) ;

int uart_read_line(char * data) ;

void uart_RX_interrupt_hanlder(uint8_t rx) ;
