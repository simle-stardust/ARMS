################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/Third_Party/FatFs/src/option/syscall.c 

OBJS += \
./Middlewares/Third_Party/FatFs/src/option/syscall.o 

C_DEPS += \
./Middlewares/Third_Party/FatFs/src/option/syscall.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/Third_Party/FatFs/src/option/%.o: ../Middlewares/Third_Party/FatFs/src/option/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -mfloat-abi=soft '-D__weak=__attribute__((weak))' '-D__packed="__attribute__((__packed__))"' -DUSE_HAL_DRIVER -DSTM32L151xD -I"/home/szymon/workspace/ARMS/Inc" -I"/home/szymon/workspace/ARMS/Drivers/STM32L1xx_HAL_Driver/Inc" -I"/home/szymon/workspace/ARMS/Drivers/STM32L1xx_HAL_Driver/Inc/Legacy" -I"/home/szymon/workspace/ARMS/Middlewares/Third_Party/FatFs/src/drivers" -I"/home/szymon/workspace/ARMS/Drivers/CMSIS/Device/ST/STM32L1xx/Include" -I"/home/szymon/workspace/ARMS/Middlewares/Third_Party/FatFs/src" -I"/home/szymon/workspace/ARMS/Drivers/CMSIS/Include" -I"/home/szymon/workspace/ARMS/Inc"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


