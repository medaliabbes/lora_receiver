

#ifndef _LINK_LAYER_H_
#define _LINK_LAYER_H_

#include "packet.h"
#include "proto_types.h"

#define  TX_PACKET_LIST_SIZE         10
#define  RX_PACKET_LIST_SIZE         10

#define  BUFFER_FULL			    (-10)
#define  ALLOCATION_FAILED          (-11)

#define  MAX_NUMBER_OF_TRANSMITION   (5) //the maximum number of time a packet is transmitted to the network 

struct packet_holder
{
    packet_t packet ;
    int transmition_time ;
    int number_of_transmition ;
} ;

typedef struct packet_holder packet_holder_t ;

//should be called first , default adress is 0xFF
int ll_init(u8 addr);

void ll_process(void) ;

int ll_send_ASK(u8 dest) ;

int ll_send_NANK(u8 dest) ;

//send a data packet 
int ll_send_to(u8 dest ,u8 *data ,int data_len) ;

//check Rx queue for data from the dest (return a payload length and copy the payload in data)
int  ll_get_recv_from(u8 dest ,u8 *data );

//do the actuel transmition , should be called from ll_process
void ll_transmit(void) ;

//get the received data ,converted into packet_t type and put it in Rx queue 
void ll_receive(void) ;

int get_tx_size() ;

void ll_debug_tx_list() ;

#endif /*_LINK_LAYER_H_*/