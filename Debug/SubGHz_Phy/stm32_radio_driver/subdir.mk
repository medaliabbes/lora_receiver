################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../middlewares/SubGHz_Phy/stm32_radio_driver/radio.c \
../middlewares/SubGHz_Phy/stm32_radio_driver/radio_driver.c \
../middlewares/SubGHz_Phy/stm32_radio_driver/radio_fw.c 

OBJS += \
./SubGHz_Phy/stm32_radio_driver/radio.o \
./SubGHz_Phy/stm32_radio_driver/radio_driver.o \
./SubGHz_Phy/stm32_radio_driver/radio_fw.o 

C_DEPS += \
./SubGHz_Phy/stm32_radio_driver/radio.d \
./SubGHz_Phy/stm32_radio_driver/radio_driver.d \
./SubGHz_Phy/stm32_radio_driver/radio_fw.d 


# Each subdirectory must supply rules for building sources it contributes
SubGHz_Phy/stm32_radio_driver/radio.o: C:/Users/medali/Desktop/project\ lora\ upwork/LoRa/middlewares/SubGHz_Phy/stm32_radio_driver/radio.c SubGHz_Phy/stm32_radio_driver/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DCORE_CM4 -DUSE_HAL_DRIVER -DSTM32WLE5xx -c -I../Core/Inc -I../Drivers/STM32WLxx_HAL_Driver/Inc -I../Drivers/STM32WLxx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32WLxx/Include -I../Drivers/CMSIS/Include -I"../Drivers/BSP/STM32WLxx_Nucleo" -I"../middlewares/SubGHz_Phy" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
SubGHz_Phy/stm32_radio_driver/radio_driver.o: C:/Users/medali/Desktop/project\ lora\ upwork/LoRa/middlewares/SubGHz_Phy/stm32_radio_driver/radio_driver.c SubGHz_Phy/stm32_radio_driver/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DCORE_CM4 -DUSE_HAL_DRIVER -DSTM32WLE5xx -c -I../Core/Inc -I../Drivers/STM32WLxx_HAL_Driver/Inc -I../Drivers/STM32WLxx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32WLxx/Include -I../Drivers/CMSIS/Include -I"../Drivers/BSP/STM32WLxx_Nucleo" -I"../middlewares/SubGHz_Phy" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
SubGHz_Phy/stm32_radio_driver/radio_fw.o: C:/Users/medali/Desktop/project\ lora\ upwork/LoRa/middlewares/SubGHz_Phy/stm32_radio_driver/radio_fw.c SubGHz_Phy/stm32_radio_driver/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DCORE_CM4 -DUSE_HAL_DRIVER -DSTM32WLE5xx -c -I../Core/Inc -I../Drivers/STM32WLxx_HAL_Driver/Inc -I../Drivers/STM32WLxx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32WLxx/Include -I../Drivers/CMSIS/Include -I"../Drivers/BSP/STM32WLxx_Nucleo" -I"../middlewares/SubGHz_Phy" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

