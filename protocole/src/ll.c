	
#include "ll.h"
#include "stdio.h"
#include "proto_types.h"
#include "packet.h"
//#include "cQueue.h"
#include "list.h"
#include "stdbool.h"
#include "stdlib.h"
#include "radio.h"



extern const struct Radio_s Radio;

//use random id in ll_send_ASK , ll_send_NANK
list_t * Rx_packet_list ;
list_t * Tx_packet_list ;

u32 network_transmit_index = 0 ;
u8 device_address = 0xFF; 


bool find_packet_by_Id(void* packet_id ,void* arg2 )
{
    int id = (int)packet_id;
    packet_t * p2 = (packet_t*)arg2;

    if(id == p2->id)
    {
        return true ;
    }
    else{
        return false ;
    }
}

int ll_init(u8 addr)
{
	device_address = addr ;

	Tx_packet_list = list_new(NULL , &free) ;
	Rx_packet_list = list_new(&find_packet_by_Id , &free) ;

	return 0 ;
}

static int ll_send_packet(u8 dest,u8 type , u8 * data ,u8 len)
{
	if(list_size(Tx_packet_list) >= TX_PACKET_LIST_SIZE )
	{
		return BUFFER_FULL ;
	}
	
	packet_t  tmp  ;
	packet_holder_t tmp_holder ;
	
	packet(&tmp , device_address , dest , type ,88,data , len);
	
	tmp_holder.packet = tmp ;
	tmp_holder.number_of_transmition = 0 ;
	
	list_push_back(Tx_packet_list , list_node_new(&tmp_holder ,sizeof(packet_holder_t))) ;
	
	(void) tmp ;
	(void) tmp_holder ;

	return 0 ;
}

int ll_send_ASK(u8 dest) 
{
	return ll_send_packet(dest , PACK_TYPE_ASK ,NULL , 0);
}

int ll_send_NANK(u8 dest) 
{
	return ll_send_packet(dest , PACK_TYPE_NANK ,NULL , 0);
}

int ll_send_to(u8 dest ,u8 *data ,int data_len) 
{
	return ll_send_packet(dest , PACK_TYPE_DATA ,data , data_len);
}

int get_tx_size()
{
	return list_size(Tx_packet_list)  ;
}

void ll_transmit(void) 
{
	//pop from TX queue , serialize and send to network
	// no data in tx list exit 
	if(list_size(Tx_packet_list) == 0)
		return ;
	// in case there is  data 


	// add timer here
	while(network_transmit_index < list_size(Tx_packet_list))
	{
		printf("network transmit\n");
		struct list_node * node = list_index(Tx_packet_list , network_transmit_index) ;
		packet_holder_t	* holder = (packet_holder_t*)node->data ;
		packet_t * pack = &(holder)->packet ;
		debug_packet(pack) ;

		//packet serialize and send to hardware 
		int packet_size = packet_get_size(pack) ;

		u8 * buff = (u8 *) malloc(sizeof(u8) * packet_size) ;

		packet_serialize(pack , buff) ;

		//send to network
		Radio.Send(buff , packet_size) ;

		//increment number of transmetion

		holder->number_of_transmition++ ;

		free(buff) ;
		
		network_transmit_index++;
		// should remove packets with number of transmition >= 2 (packet transmited 3 time) 
		
		// can define a time out for the packet to leave the list 

		// small delay or wait for TxDone interrupt
		sys_delay(100) ;

		// remove the packet that sent equal or more than the maximum
		if(holder->number_of_transmition >= MAX_NUMBER_OF_TRANSMITION)
		{
			//free memory
			free(pack) ;
			free(holder);
			list_remove(Tx_packet_list , node) ;
			free(node);
			printf("packet removed \ndeallocating memory\n");
		}
	}


	if(network_transmit_index >= list_size(Tx_packet_list))
	{
		network_transmit_index = 0 ;	
	}
}

void ll_debug_tx_list()
{
	for(int i = 0 ; i< list_size(Tx_packet_list) ;i++)
	{
		struct list_node * n = list_index(Tx_packet_list , i);
		packet_holder_t * tmp =(packet_holder_t*) n->data ;
		debug_packet(&(tmp)->packet) ;
	}
}
