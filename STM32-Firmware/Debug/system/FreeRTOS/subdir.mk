################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../system/FreeRTOS/croutine.c \
../system/FreeRTOS/heap_4.c \
../system/FreeRTOS/list.c \
../system/FreeRTOS/port.c \
../system/FreeRTOS/queue.c \
../system/FreeRTOS/tasks.c \
../system/FreeRTOS/timers.c 

OBJS += \
./system/FreeRTOS/croutine.o \
./system/FreeRTOS/heap_4.o \
./system/FreeRTOS/list.o \
./system/FreeRTOS/port.o \
./system/FreeRTOS/queue.o \
./system/FreeRTOS/tasks.o \
./system/FreeRTOS/timers.o 

C_DEPS += \
./system/FreeRTOS/croutine.d \
./system/FreeRTOS/heap_4.d \
./system/FreeRTOS/list.d \
./system/FreeRTOS/port.d \
./system/FreeRTOS/queue.d \
./system/FreeRTOS/tasks.d \
./system/FreeRTOS/timers.d 


# Each subdirectory must supply rules for building sources it contributes
system/FreeRTOS/%.o: ../system/FreeRTOS/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -Wall -Wextra  -g3 -DDEBUG -DUSE_FULL_ASSERT -DSTM32F2XX -DUSE_STDPERIPH_DRIVER -DHSE_VALUE=8000000 -I"../include" -I"/home/kapitan/Dokumenty/ELEKTRONIKA/ESP8266-WebRadio/STM32-Firmware/system/FreeRTOS/include" -I"/home/kapitan/Dokumenty/ELEKTRONIKA/ESP8266-WebRadio/STM32-Firmware/system/STM32F2xx_HAL_Driver/Inc" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f2-stdperiph" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


