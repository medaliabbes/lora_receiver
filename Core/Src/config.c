

#include "main.h"
#include "config.h"
#include "string.h"
#include "stdlib.h"

void config_init()
{
  FLASH_WaitForLastOperation(1000);
  HAL_FLASH_Unlock() ;
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);
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

void config_load(config_t * param)
{
	uint8_t * p_mydata = malloc(sizeof(config_t)) ;

	uint32_t adress = FLASH_USER_START_ADDR ;

	for(int i = 0 ; i<sizeof(config_t) ; i++)
	{
	  p_mydata[i] =*(uint8_t*)(adress);

	  adress++ ;
	}
	memcpy(param , p_mydata ,sizeof(config_t) ) ;

	free(p_mydata) ;
}

int load_nodes(saved_nodes_t * nodes)
{
	uint8_t * p_mydata = malloc(sizeof(saved_nodes_t)) ;

	uint32_t adress = FLASH_USER_START_ADDR + 8;

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

int store_nodes(saved_nodes_t * nodes)
{
	uint32_t  PAGEError = 0;

	//static FLASH_EraseInitTypeDef EraseInitStruct;
	FLASH_EraseInitTypeDef EraseInitStruct;
	HAL_StatusTypeDef sta ;

	uint32_t adress = FLASH_USER_START_ADDR + 8;
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

