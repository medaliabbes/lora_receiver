################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../protocole/src/list.c \
../protocole/src/ll.c \
../protocole/src/packet.c 

OBJS += \
./protocole/src/list.o \
./protocole/src/ll.o \
./protocole/src/packet.o 

C_DEPS += \
./protocole/src/list.d \
./protocole/src/ll.d \
./protocole/src/packet.d 


# Each subdirectory must supply rules for building sources it contributes
protocole/src/%.o: ../protocole/src/%.c protocole/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DCORE_CM4 -DUSE_HAL_DRIVER -DSTM32WLE5xx -c -I../Core/Inc -I../Drivers/STM32WLxx_HAL_Driver/Inc -I../Drivers/STM32WLxx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32WLxx/Include -I../Drivers/CMSIS/Include -I"../Drivers/BSP/STM32WLxx_Nucleo" -I"../middlewares/SubGHz_Phy" -I"C:/Users/medali/Desktop/project lora upwork/LoRa/protocole/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

