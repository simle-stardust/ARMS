################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/adc.c \
../Src/bsp_driver_sd.c \
../Src/comp.c \
../Src/dac.c \
../Src/fatfs.c \
../Src/gpio.c \
../Src/main.c \
../Src/rtc.c \
../Src/sdio.c \
../Src/stm32l1xx_hal_msp.c \
../Src/stm32l1xx_it.c \
../Src/system_stm32l1xx.c \
../Src/tim.c \
../Src/usart.c 

OBJS += \
./Src/adc.o \
./Src/bsp_driver_sd.o \
./Src/comp.o \
./Src/dac.o \
./Src/fatfs.o \
./Src/gpio.o \
./Src/main.o \
./Src/rtc.o \
./Src/sdio.o \
./Src/stm32l1xx_hal_msp.o \
./Src/stm32l1xx_it.o \
./Src/system_stm32l1xx.o \
./Src/tim.o \
./Src/usart.o 

C_DEPS += \
./Src/adc.d \
./Src/bsp_driver_sd.d \
./Src/comp.d \
./Src/dac.d \
./Src/fatfs.d \
./Src/gpio.d \
./Src/main.d \
./Src/rtc.d \
./Src/sdio.d \
./Src/stm32l1xx_hal_msp.d \
./Src/stm32l1xx_it.d \
./Src/system_stm32l1xx.d \
./Src/tim.d \
./Src/usart.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o: ../Src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -mfloat-abi=soft '-D__weak=__attribute__((weak))' '-D__packed="__attribute__((__packed__))"' -DUSE_HAL_DRIVER -DSTM32L151xD -I"/home/szymon/workspace/ARMS/Inc" -I"/home/szymon/workspace/ARMS/Drivers/STM32L1xx_HAL_Driver/Inc" -I"/home/szymon/workspace/ARMS/Drivers/STM32L1xx_HAL_Driver/Inc/Legacy" -I"/home/szymon/workspace/ARMS/Middlewares/Third_Party/FatFs/src/drivers" -I"/home/szymon/workspace/ARMS/Drivers/CMSIS/Device/ST/STM32L1xx/Include" -I"/home/szymon/workspace/ARMS/Middlewares/Third_Party/FatFs/src" -I"/home/szymon/workspace/ARMS/Drivers/CMSIS/Include" -I"/home/szymon/workspace/ARMS/Inc"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


