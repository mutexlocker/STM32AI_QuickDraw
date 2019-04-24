################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/BSP/Components/ili9325/ili9325.c 

OBJS += \
./Drivers/BSP/Components/ili9325/ili9325.o 

C_DEPS += \
./Drivers/BSP/Components/ili9325/ili9325.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/Components/ili9325/%.o: ../Drivers/BSP/Components/ili9325/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed="__attribute__((__packed__))"' -DARM_MATH_CM4 '-D__FPU_PRESENT=1U' -DUSE_HAL_DRIVER -DSTM32F429xx -I"/Users/nimaaghli/Documents/workspace/STM32AI_QuickDraw/Inc" -I"/Users/nimaaghli/Documents/workspace/STM32AI_QuickDraw/Middlewares/ST/AI/AI/data" -I"/Users/nimaaghli/Documents/workspace/STM32AI_QuickDraw/Middlewares/ST/AI/AI/include" -I"/Users/nimaaghli/Documents/workspace/STM32AI_QuickDraw/Drivers/CMSIS/DSP/Include" -I"/Users/nimaaghli/Documents/workspace/STM32AI_QuickDraw/Drivers/STM32F4xx_HAL_Driver/Inc" -I"/Users/nimaaghli/Documents/workspace/STM32AI_QuickDraw/SW4STM32/STM32AI_QuickDraw/Drivers/BSP/STM32F429I-Discovery" -I"/Users/nimaaghli/Documents/workspace/STM32AI_QuickDraw/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy" -I"/Users/nimaaghli/Documents/workspace/STM32AI_QuickDraw/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"/Users/nimaaghli/Documents/workspace/STM32AI_QuickDraw/Drivers/CMSIS/Include"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


