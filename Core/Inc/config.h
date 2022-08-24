
#include "main.h"


#define 	CONFIG_OK      0x00
#define 	CONFIG_ERROR   0x01
#define     VALIDE_CONFIGURATION	 (0x4ea1c1)


#define ADDR_FLASH_PAGE_127     ((uint32_t)0x0803F800)
#define FLASH_USER_START_ADDR   ADDR_FLASH_PAGE_127



 __attribute__((packed))struct config{
	int periode ;
	float seuil ;
};


 __attribute__((packed))struct saved_nodes{
	 uint8_t valide ;
	 uint8_t nb_addresses ;
	 uint8_t addresses[6] ;
 };

typedef struct config config_t ;
typedef struct saved_nodes saved_nodes_t ;

//should be called before using any of the library function
void config_init(void) ;

//used by receiver to save maximum flow and sending period
//int config_save(config_t * param) ;

int config_load(config_t * param) ;

//used by transmitter to store number of nodes and they addresses
int load_nodes(saved_nodes_t * nodes) ;

//int store_nodes(saved_nodes_t * nodes) ;

int config_save1(config_t * param) ;

int  store_nodes1(saved_nodes_t * nodes) ;
