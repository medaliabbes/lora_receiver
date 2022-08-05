

#ifndef  _PACKET_H_
#define  _PACKET_H_

#include "proto_types.h"


#define  PACK_TYPE_DATA    0x01  // a data packet
#define  PACK_TYPE_ASK	   0x02  // packet recieved 
#define  PACK_TYPE_NANK	   0x03  // packet corrept

#define  PACK_OK				 (0)
#define  PACK_CHECKSUM_ERROR     (-1)
#define  PACK_HEADER_LENGTH		  7 //header + check sum


struct packet {
	u8 src;
	u8 dest;
	u8 id ;
	u8 type;
	u8 payload_length;
	u8* payload;
} ;

typedef struct packet packet_t ;

void packet(packet_t * pack ,u8 src ,u8 dest ,u8 type ,u8 id, u8* data ,u8 len) ;

void packet_set_src(packet_t *pack ,u8 src);

void packet_set_dest(packet_t *pack ,u8 dest);

void packet_set_type(packet_t *pack ,u8 type) ;

void packet_set_payload(packet_t *pack ,u8  *payload  , int len) ;

void packet_set_id(packet_t *pack ,u8 id) ;

void debug_packet(packet_t * pack);

u16 check_sum(u8 * buffer , int len) ;

// transform the packet into a buffer to be transmitted 
// should be called whene the packet is about to be send (the check sum is calculated here)
int packet_serialize(packet_t * pack ,u8 * buffer ) ;

//construct the packet from its buffer return -1 in case of error 
// packet checksum is verified here
int packet_desirialize(u8 * buffer,int buffer_len , packet_t * pack) ;

int packet_get_size(packet_t * pack) ;

#endif /* _PACKET_H_ */
