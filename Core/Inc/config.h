
#include "main.h"


#define 	CONFIG_OK      0x00
#define 	CONFIG_ERROR   0x01



#define ADDR_FLASH_PAGE_127     ((uint32_t)0x0803F800)
#define FLASH_USER_START_ADDR   ADDR_FLASH_PAGE_127



 __attribute__((packed))struct config{
	int periode ;
	float seuil ;
};

 typedef struct config config_t ;

void config_init(void) ;

int config_save(config_t * param) ;

void config_load(config_t * param) ;
