################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/BSP/STM32F429I-Discovery/stm32f429i_discovery.c \
../Drivers/BSP/STM32F429I-Discovery/stm32f429i_discovery_eeprom.c \
../Drivers/BSP/STM32F429I-Discovery/stm32f429i_discovery_io.c \
../Drivers/BSP/STM32F429I-Discovery/stm32f429i_discovery_lcd.c \
../Drivers/BSP/STM32F429I-Discovery/stm32f429i_discovery_sdram.c \
../Drivers/BSP/STM32F429I-Discovery/stm32f429i_discovery_ts.c 

OBJS += \
./Drivers/BSP/STM32F429I-Discovery/stm32f429i_discovery.o \
./Drivers/BSP/STM32F429I-Discovery/stm32f429i_discovery_eeprom.o \
./Drivers/BSP/STM32F429I-Discovery/stm32f429i_discovery_io.o \
./Drivers/BSP/STM32F429I-Discovery/stm32f429i_discovery_lcd.o \
./Drivers/BSP/STM32F429I-Discovery/stm32f429i_discovery_sdram.o \
./Drivers/BSP/STM32F429I-Discovery/stm32f429i_discovery_ts.o 

C_DEPS += \
./Drivers/BSP/STM32F429I-Discovery/stm32f429i_discovery.d \
./Drivers/BSP/STM32F429I-Discovery/stm32f429i_discovery_eeprom.d \
./Drivers/BSP/STM32F429I-Discovery/stm32f429i_discovery_io.d \
./Drivers/BSP/STM32F429I-Discovery/stm32f429i_discovery_lcd.d \
./Drivers/BSP/STM32F429I-Discovery/stm32f429i_discovery_sdram.d \
./Drivers/BSP/STM32F429I-Discovery/stm32f429i_discovery_ts.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/STM32F429I-Discovery/%.o: ../Drivers/BSP/STM32F429I-Discovery/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed="__attribute__((__packed__))"' -DARM_MATH_CM4 '-D__FPU_PRESENT=1U' -DUSE_HAL_DRIVER -DSTM32F429xx -I"/Users/nimaaghli/Documents/workspace/STM32AI_QuickDraw/Inc" -I"/Users/nimaaghli/Documents/workspace/STM32AI_QuickDraw/Middlewares/ST/AI/AI/data" -I"/Users/nimaaghli/Documents/workspace/STM32AI_QuickDraw/Middlewares/ST/AI/AI/include" -I"/Users/nimaaghli/Documents/workspace/STM32AI_QuickDraw/Drivers/CMSIS/DSP/Include" -I"/Users/nimaaghli/Documents/workspace/STM32AI_QuickDraw/Drivers/STM32F4xx_HAL_Driver/Inc" -I"/Users/nimaaghli/Documents/workspace/STM32AI_QuickDraw/SW4STM32/STM32AI_QuickDraw/Drivers/BSP/STM32F429I-Discovery" -I"/Users/nimaaghli/Documents/workspace/STM32AI_QuickDraw/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy" -I"/Users/nimaaghli/Documents/workspace/STM32AI_QuickDraw/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"/Users/nimaaghli/Documents/workspace/STM32AI_QuickDraw/Drivers/CMSIS/Include"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


