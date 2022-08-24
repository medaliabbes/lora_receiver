

#include "main.h"
#include "config.h"
#include "string.h"
#include "stdlib.h"

#include "stdio.h"

/*
 * this function will read 3 * 64 bit from the address FLASH_USER_START_ADDR
 */
static void flash_read_storage(uint8_t * buffer  )
{
	uint32_t adress = FLASH_USER_START_ADDR ;

	for(int i = 0 ; i< 24 ; i++)
	{
	  buffer[i] =*(uint8_t*)(adress);

	  adress++ ;
	}

}

static int flash_write_storage(uint8_t * buffer)
{

	uint32_t  PAGEError = 0;

	FLASH_EraseInitTypeDef EraseInitStruct;
	HAL_StatusTypeDef sta ;
	uint32_t adress = FLASH_USER_START_ADDR ;


	EraseInitStruct.TypeErase =  FLASH_TYPEERASE_PAGES ;
	EraseInitStruct.Page      =  FLASH_PAGE_NB - 1     ;
	EraseInitStruct.NbPages   =  1 ;

	HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) ;


	uint64_t mydata ;


	memcpy(&mydata , &buffer[0] , sizeof(uint64_t));

	sta = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD ,adress , mydata);

	if(sta != HAL_OK )
	{
	  return CONFIG_ERROR ;
	}
	adress += 8 ;

	memcpy(&mydata , &buffer[8] , sizeof(uint64_t));

	sta = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD ,adress , mydata);

	if(sta != HAL_OK )
	{
	  return CONFIG_ERROR ;
	}
	adress += 8 ;

	memcpy(&mydata , &buffer[16] , sizeof(uint64_t));

	sta = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD ,adress , mydata);

	if(sta != HAL_OK )
	{
	  return CONFIG_ERROR ;
	}

	return CONFIG_OK ;

}

void config_init()
{
	  FLASH_WaitForLastOperation(1000);
	  HAL_FLASH_Unlock() ;
	  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);
}

int config_save1(config_t * param)
{

	uint8_t buffer[24] ;

	flash_read_storage(buffer) ;

	memcpy(&buffer[0] ,  param , sizeof(config_t) );

	uint64_t key = VALIDE_CONFIGURATION ;

	memcpy(&buffer[8] , &key , sizeof(uint64_t));

	int sta = flash_write_storage(buffer) ;

	(void) buffer ;

	return sta ;
}

int config_save(config_t * param)
{
	  uint32_t  PAGEError = 0;

	  //static FLASH_EraseInitTypeDef EraseInitStruct;
	  FLASH_EraseInitTypeDef EraseInitStruct;
	  HAL_StatusTypeDef sta ;

	  uint32_t adress = FLASH_USER_START_ADDR ;
	  //config_t myconfig = *param ;
	  uint64_t mydata  ;

	  memcpy(&mydata ,param , (int)sizeof(config_t) ) ;

	  EraseInitStruct.TypeErase =  FLASH_TYPEERASE_PAGES ;
	  EraseInitStruct.Page      =  FLASH_PAGE_NB - 1     ;
	  EraseInitStruct.NbPages   =  1 ;

	  HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) ;

	  sta = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD ,adress , mydata);

	  if(sta != HAL_OK )
	  {
		  return CONFIG_ERROR ;
	  }

	  //this to validate config ; what is in the flash is valid data
	  mydata = VALIDE_CONFIGURATION ;

	  sta = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD ,adress + 8, mydata);

	  HAL_FLASH_Lock() ;

	  if(sta != HAL_OK )
	  {
		  return CONFIG_ERROR ;
	  }
	  else
	  {
		  return CONFIG_OK ;
	  }

}

int config_erase()
{
	return 0 ;
}


int config_load(config_t * param)
{
	uint8_t * p_mydata = malloc(sizeof(config_t)) ;

	uint32_t adress = FLASH_USER_START_ADDR ;

	for(int i = 0 ; i<sizeof(config_t) ; i++)
	{
	  p_mydata[i] =*(uint8_t*)(adress);

	  adress++ ;
	}

	memcpy(param , p_mydata ,sizeof(config_t) ) ;


	//read from the validation address
	adress = FLASH_USER_START_ADDR + 8 ;

	for(int i = 0 ; i<sizeof(config_t) ; i++)
	{
	  p_mydata[i] =*(uint8_t*)(adress);

	  adress++ ;
	}

	uint64_t key ;

	memcpy(&key , p_mydata ,sizeof(uint64_t) ) ;

	free(p_mydata) ;

	if(key == VALIDE_CONFIGURATION)
	{
		return CONFIG_OK ;
	}
	else
	{
		return CONFIG_ERROR ;
	}
}

int load_nodes(saved_nodes_t * nodes)
{
	uint8_t * p_mydata = malloc(sizeof(saved_nodes_t)) ;

	uint32_t adress = FLASH_USER_START_ADDR + 16;

	for(int i = 0 ; i<sizeof(saved_nodes_t) ; i++)
	{
	  p_mydata[i] =*(uint8_t*)(adress);

	  adress++ ;
	}
	memcpy(nodes , p_mydata ,sizeof(saved_nodes_t) ) ;

	free(p_mydata) ;

	if(nodes->valide != 1)
	{
		return CONFIG_ERROR ;
	}
	else
	{
		return CONFIG_OK ;
	}
}


int store_nodes1(saved_nodes_t * nodes)
{
	uint8_t buffer[24] ;

	flash_read_storage(buffer) ;

	memcpy(&buffer[16] ,  nodes , sizeof(saved_nodes_t) );

	int sta = flash_write_storage(buffer) ;

	(void) buffer ;

	return sta ;
}

int store_nodes(saved_nodes_t * nodes)
{
	uint32_t  PAGEError = 0;

	//static FLASH_EraseInitTypeDef EraseInitStruct;
	FLASH_EraseInitTypeDef EraseInitStruct;
	HAL_StatusTypeDef sta ;

	uint32_t adress = FLASH_USER_START_ADDR + 16;
	//config_t myconfig = *param ;
	uint64_t mydata  ;

	memcpy(&mydata ,nodes , (int)sizeof(saved_nodes_t) ) ;

	EraseInitStruct.TypeErase =  FLASH_TYPEERASE_PAGES ;
	EraseInitStruct.Page      =  FLASH_PAGE_NB - 1     ;
	EraseInitStruct.NbPages   =  1 ;

	HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) ;

	sta = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD ,adress , mydata);

	HAL_FLASH_Lock() ;

	if(sta != HAL_OK )
	{
	  return CONFIG_ERROR ;
	}
	else
	{
	  return CONFIG_OK ;
	}
}

