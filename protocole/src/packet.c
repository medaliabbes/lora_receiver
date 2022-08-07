

#include "packet.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

void packet(packet_t * pack ,u8 src ,u8 dest ,u8 type ,u8 id, u8* data ,u8 len)
{
	//set packet header
	packet_set_src (pack  , src) ;
	packet_set_dest(pack  , dest) ;
	packet_set_type(pack , type );
	packet_set_id(pack ,id) ;
	packet_set_payload(pack ,data , len) ;
}

void packet_set_src(packet_t *pack ,u8 src)
{
	pack->src = src ;
}

void packet_set_dest(packet_t *pack ,u8 dest)
{
	pack->dest = dest ;
}

void packet_set_type(packet_t *pack ,u8 type) 
{
	pack->type = type ;
}

void packet_set_payload(packet_t *pack ,u8 * payload , int len) 
{
	if(payload != NULL && len != 0)
	{
		pack->payload = (u8*) malloc(sizeof(u8) * len) ;//payload ;
		memcpy(pack->payload , payload , len) ;
		pack->payload_length = len ;
		return ;
	}
	pack->payload_length = 0 ;
	
}

void packet_set_id(packet_t *pack ,u8 id)
{
	pack->id = id ;
}

void debug_packet(packet_t * pack)
{
	printf("PAcket :\n");
	printf("src  ->%d\n" , pack->src);
	printf("dest ->%d\n" , pack->dest);
	printf("id   ->%d\n" , pack->id) ;
	if(pack->type == PACK_TYPE_DATA)
	{
		printf("packet type data\n") ;
	}
	else if(pack->type == PACK_TYPE_ASK)
	{
		printf("packet type ask\n") ;
	}
	else if(pack->type == PACK_TYPE_NANK)
	{
		printf("packet type nank\n") ;
	}
	else{
		printf("packet type not defined\n") ;
	}
	
	
	printf("packet length :%d\n" , pack->payload_length );
	printf("payload :");
	for(int i = 0 ; i<pack->payload_length ;i++)
	{
		printf("%c" , pack->payload[i]);
	}
	printf("\n");
	
}

int packet_serialize(packet_t * pack ,u8 * buffer ) 
{
	/* buffer size = 1 byte src + 1 byte dest + 1 byte type + 
	1 byte payload_length + payload_length + 2 byte checksum*/
	int buffer_size  = 7 + pack->payload_length ;
	
	//buffer = (u8*)malloc(sizeof(u8) * buffer_size) ;
	if( buffer != NULL)
	{
		u8 *pbuffer = buffer ;
		*pbuffer = pack->dest ;
		pbuffer++;
		*pbuffer = pack->src ;
		pbuffer++;
		*pbuffer = pack->id ;
		pbuffer++;
		*pbuffer = pack->type ;
		pbuffer++;
		*pbuffer = pack->payload_length ;
		pbuffer++;
		memcpy(pbuffer , pack->payload , pack->payload_length );
		u16 checksum = check_sum(buffer , 5 + pack->payload_length );
		pbuffer += pack->payload_length ;
		*pbuffer = MSB16(checksum) ;
		pbuffer++;
		*pbuffer = LSB16(checksum) ;
		return buffer_size ;
	}
	else
	{
		return 0;
	}
}

int packet_desirialize(u8 * buffer,int buffer_len , packet_t * pack) 
{
	u16 pack_checksum = (buffer[buffer_len -2] << 8)& 0xff00 ;
	pack_checksum += buffer[buffer_len -1] ;
		
	u16 cal_checksum  = check_sum(buffer , buffer_len -2) ;
	
	//printf("cal %x , pac %x\n" , cal_checksum , pack_checksum);
	if(cal_checksum != pack_checksum)
	{
		printf("\nCHECKSUM ERROR\n") ;
		return PACK_CHECKSUM_ERROR ;
	}

	
	pack->src  = buffer[1] ;
	pack->dest = buffer[0] ;
	pack->type = buffer[3] ;
	pack->id   = buffer[2] ;
	pack->payload_length = buffer[4] ;
	pack->payload = (u8*) malloc(sizeof(u8) * pack->payload_length) ; 
	memcpy(pack->payload , &buffer[5] , pack->payload_length ) ;
	
	return PACK_OK ;
}

unsigned short check_sum(u8 * buffer , int len)
{	
	int i = 0 ;
	
	int sum = 0;
	
	while(len>1)
	{
		sum += (buffer[i] << 8) & 0xff00 ;
		sum +=  buffer[i+1] ;
	
		len -= 2 ;
		i += 2;
	}
	
	if(len >0)
	{
		sum += buffer[len -1 ] ;
	}
	
	while(sum > 0xffff)
	{
		sum = sum & 0x0000ffff ;
		unsigned short carry = 0xffff&(sum >> 16 );
		sum += carry ;
	}
	
	return (unsigned short)(0xffff - sum) ;
}


int packet_get_size(packet_t * pack)
{
	return PACK_HEADER_LENGTH + pack->payload_length ;
}
