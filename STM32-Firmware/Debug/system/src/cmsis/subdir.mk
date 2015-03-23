################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../system/src/cmsis/system_stm32f2xx.c \
../system/src/cmsis/vectors_stm32f2xx.c 

OBJS += \
./system/src/cmsis/system_stm32f2xx.o \
./system/src/cmsis/vectors_stm32f2xx.o 

C_DEPS += \
./system/src/cmsis/system_stm32f2xx.d \
./system/src/cmsis/vectors_stm32f2xx.d 


# Each subdirectory must supply rules for building sources it contributes
system/src/cmsis/%.o: ../system/src/cmsis/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -Wall -Wextra  -g3 -DDEBUG -DUSE_FULL_ASSERT -DSTM32F2XX -DUSE_STDPERIPH_DRIVER -DHSE_VALUE=8000000 -I"../include" -I"/home/kapitan/Dokumenty/ELEKTRONIKA/ESP8266-WebRadio/STM32-Firmware/system/FreeRTOS/include" -I"/home/kapitan/Dokumenty/ELEKTRONIKA/ESP8266-WebRadio/STM32-Firmware/system/STM32F2xx_HAL_Driver/Inc" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f2-stdperiph" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


