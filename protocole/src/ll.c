	
#include "ll.h"
#include "stdio.h"
#include "string.h"
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

u32 network_transmit_index = 0x00 ;
u32 rx_packet_index = 0x00 ;

u32 number_of_received_packet = 0;

u8  device_address         = 0xFF ;

bool tarnsmition_done = false ;

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

bool rx_find_by_id(void * pid , void * arg2)
{
	int id = (int)pid;
	packet_t * p2 = (packet_t*)arg2;

	if(id == p2->id)
	{
		return true ;
	}
	else{
		return false ;
	}
}

/**
 * initialize Rx and Tx lists of packets
 * and mode device in Receive Mode
 */
int ll_init(u8 addr)
{
	device_address = addr ;

	Tx_packet_list = list_new(&find_packet_by_Id , &free) ;//find packet by id to remove it later
	Rx_packet_list = list_new(&rx_find_by_id , &free) ;//find packet by src


	Radio.Rx(1000);

	return 0 ;
}

static int ll_send_packet(u8 dest,u8 type ,u8 id, u8 * data ,u8 len)
{
	if(list_size(Tx_packet_list) >= TX_PACKET_LIST_SIZE )
	{
		return BUFFER_FULL ;
	}
	
	packet_t  tmp  ;
	packet_holder_t tmp_holder ;
	
	packet(&tmp , device_address , dest , type ,id,data , len);
	
	tmp_holder.packet = tmp ;
	tmp_holder.number_of_transmition = 0 ;
	
	list_push_back(Tx_packet_list , list_node_new(&tmp_holder ,sizeof(packet_holder_t))) ;
	
	(void) tmp ;
	(void) tmp_holder ;

	return 0 ;
}

int ll_send_ASK(u8 dest ,u8 id)
{
	return ll_send_packet(dest , PACK_TYPE_ASK ,id ,NULL , 0);
}

int ll_send_NANK(u8 dest,u8 id)
{
	return ll_send_packet(dest , PACK_TYPE_NANK,id ,NULL , 0);
}

int ll_send_to(u8 dest ,u8 *data ,int data_len) 
{
	u8 id = sys_random() ;

	return ll_send_packet(dest , PACK_TYPE_DATA,id ,data , data_len);
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

	printf("\nNetwork Transmit number of Packet %d\n" ,list_size(Tx_packet_list) );
	// add timer here
	while(network_transmit_index < list_size(Tx_packet_list))
	{

		struct list_node * node = list_index(Tx_packet_list , network_transmit_index) ;
		packet_holder_t	* holder = (packet_holder_t*)node->data ;
		packet_t * pack = &(holder)->packet ;
		debug_packet(pack) ;

		//packet serialize and send to hardware 
		int packet_size = packet_get_size(pack) ;

		u8 * buff = (u8 *) malloc(sizeof(u8) * packet_size) ;

		packet_serialize(pack , buff) ;

		//send to network
		if(buff != NULL)
		{
			Radio.Send(buff , packet_size) ;
			//wait for the transmition to complete
			while(tarnsmition_done == false);
		}
		else if(buff == NULL)
		{
			printf("\npacket_serialize FAILED\n");
		}
		// small delay or wait for TxDone interrupt

		//increment number of transmition

		holder->number_of_transmition++ ;

		// small delay so the buffer get transmitted then release the memory
		//sys_delay(100) ;


		free(buff) ;
		
		tarnsmition_done = false ;
		/*
		 * if packet is a NANK then remove the packet from Tx list
		 */
		printf("\nclear tx done for next packet\n");
		if(pack->type == PACK_TYPE_NANK)
		{
			printf("\rm NANK Packet\n");
			list_remove(Tx_packet_list ,node) ;
			//free(pack->payload) ;
			free(pack) ;
			free(holder);
			free(node) ;
		}

		printf("incriment p t\n");
		network_transmit_index++;
		// should remove packets with number of transmition >= 2 (packet transmited 3 time) 
		
		// can define a time out for the packet to leave the list 

		// remove the packet that sent equal or more than the maximum
		if(holder->number_of_transmition >= MAX_NUMBER_OF_TRANSMITION)
		{
			printf("\nfree memory\n");

			list_remove(Tx_packet_list , node) ;
			if(pack->type == PACK_TYPE_DATA)
			{
				free(pack->payload) ;
			}

			free(pack) ;
			free(holder);
			free(node);
			printf("packet removed \ndeallocating memory\n");
		}
	}
	printf("\nNetwork Transmit Done\n");

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


void ll_receive(u8 * payload , int size)
{

	//should check packet for duplication and may be respond to corrupt packet
	packet_t p;
	if(packet_desirialize(payload , size , &p) == PACK_OK)
	{
		//if the packet is for this device add it to rx list
		if(p.dest == device_address && list_size(Rx_packet_list) < RX_PACKET_LIST_SIZE )
		{
			//remove duplication
			if(list_size(Rx_packet_list) > 0)
			{
				struct list_node *n = list_search(Rx_packet_list,(void*)p.id) ;
				if(n != NULL )
				{
					packet_holder_t * holder = n->data ;
					packet_t * pack = (packet_t*) &(holder)->packet ;

					if((pack->type == p.type) && (pack->payload_length == p.payload_length))
					{
						//duplication
						printf("duplication \n");
						free(p.payload);
						return ;
					}
				}
			}

			packet_holder_t packet_holder;
			packet_holder.packet = p ;
			packet_holder.recv_time = sys_get_tick() ;
			packet_holder.number_of_transmition = 0 ;
			list_push_back(Rx_packet_list , list_node_new(&packet_holder , sizeof(packet_holder_t))) ;
		}
		else
		{

		}
	}

	else
	{
		printf("packet corrupt\n");
	}
	(void) p ;

	number_of_received_packet++; // this variable to count number of coming packet

}

void ll_process_received()
{
	//can add a timer to break the loop
	//parse rx list for ask and nank packet or data packets
	while(list_size(Rx_packet_list) > rx_packet_index)
	{

		struct list_node * n = list_index(Rx_packet_list ,rx_packet_index);
		packet_holder_t * holder = (packet_holder_t *) n->data ;

		packet_t * packet = (packet_t *) &(holder)->packet ;

		//debug_packet(packet) ;


		if(packet->type == PACK_TYPE_DATA)
		{
			// ASK packet should be sent ,the data will be read later
			// by recv_from function ,also packet should be removed after certain time
			printf("data packet\n");

			//number_of_transmition used to assure that a ASK has been send to confirm data recv
			//data packet will be removed by ll_get_recv_from
			if( holder->number_of_transmition == 0)
			{
				printf("ASK has been send for id = %d\n" , packet->id);
				ll_send_ASK(packet->src , packet->id) ;
				holder->number_of_transmition = 1 ;
			}

		}

		//ASK confirm Data recv , so first look for a data match in tx list
		else if(packet->type == PACK_TYPE_ASK)
		{
			// search for packet by id in tx list
			struct list_node * tx_data_node = list_search(Tx_packet_list ,(void*) packet->id) ;

			//if no match in tx list drop the packet
			if(tx_data_node == NULL )
			{
				printf("\nDROP ASK PACKET\n");
				//drop the ASK packet from Rx list
				list_remove(Rx_packet_list , n);

				//free memory
				free(packet->payload) ;
				free(packet) ;
				free(holder) ;
				free(n);
			}

			//in case of match in tx list ,send a NANK and remove the data packet from tx list
			else
			{
				printf("ASK match data\n");
				//send NANK
				ll_send_NANK(packet->src , packet->id) ;
				//remove data from Tx list to do not get send again

				list_remove( Tx_packet_list ,tx_data_node) ;

				packet_holder_t* h =(packet_holder_t*)tx_data_node->data ;
				packet_t * pack_p = (packet_t *)&(h->packet);
				//free memory
				free(pack_p->payload) ;
				free(pack_p) ;
				free(h);
				free(tx_data_node) ;
				(void) h;
				(void) pack_p ;
			}


		}

		else if(packet->type == PACK_TYPE_NANK)
		{
			// check the packet id in Tx list if exist resent the data and remove the packet
			// NANK indicate data received by receiver
			printf("\nNANK RECV\n");

			struct list_node * tx_ask_node = list_search(Tx_packet_list ,(void*) packet->id) ;

			if(tx_ask_node != NULL)
			{
				packet_holder_t * p_holder = tx_ask_node->data ;
				packet_t * ask_packet = &(p_holder)->packet ;
				if( ask_packet->type == PACK_TYPE_ASK )
				{
					//remove the ask packet from
					printf("transmition end\n");

					list_remove(Tx_packet_list , tx_ask_node) ;

					//release memory

					free(ask_packet);
					free(p_holder) ;
					free(tx_ask_node) ;

				}
			}
			else
			{
				// in case of null remove the packet
			}

			// in case of NANK remove the packet from Rx list
			list_remove(Rx_packet_list , n) ;
			free(packet);
			free(holder);
			free(n);
		}

		rx_packet_index++;

		printf("rx list size %d\n",list_size(Rx_packet_list)) ;

		(void) packet ;
		(void) holder ;
		(void) n ;
	}

	if(rx_packet_index > list_size(Rx_packet_list))
	{
		rx_packet_index = 0;
	}

}

int  ll_get_recv_from(u8 src ,u8 *data )
{


	int rx_list_size = list_size(Rx_packet_list) ;



	if(rx_list_size == 0)
		return 0 ;

	struct list_node* node ;
	packet_holder_t * holder ;
	packet_t * pack ;

	for(int i = 0 ; i < rx_list_size ; i++)
	{
		node = list_index(Rx_packet_list , i) ;
		holder = (packet_holder_t *) node->data ;
		pack = (packet_t*)&(holder)->packet ;

		if(pack->type == PACK_TYPE_DATA && pack->src == src)
		{
			int data_length =  pack->payload_length ;
			memcpy(data , pack->payload , data_length) ;

			if( holder->number_of_transmition == 0)
			{
				printf("ASK send From ll_get_recv_from\n");
				ll_send_ASK(pack->src , pack->id) ;
			}
			//remove the node and free memory
			list_remove(Rx_packet_list , node) ;
			free(pack->payload) ;
			free(pack) ;
			free(holder) ;
			free(node) ;
			return data_length ;
		}
	}
	// navigated the list and no data packet from src ,return 0
	return 0 ;
}


u32 process_tmr = 0;
//this function should manage send and receive operations
void ll_process(void)
{
	ll_process_received();
    if(sys_get_tick() - process_tmr > 1000)
    {
	    ll_transmit() ;
	    Radio.Rx(1000);
	    process_tmr = sys_get_tick() ;
    }
}


void ll_set_transmition_done()
{
	tarnsmition_done = true ;
}
