################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
/Users/nimaaghli/Documents/workspace/STM32AI_QuickDraw/SW4STM32/startup_stm32f429xx.s 

C_SRCS += \
/Users/nimaaghli/Documents/workspace/STM32AI_QuickDraw/SW4STM32/syscalls.c 

OBJS += \
./Application/SW4STM32/startup_stm32f429xx.o \
./Application/SW4STM32/syscalls.o 

C_DEPS += \
./Application/SW4STM32/syscalls.d 


# Each subdirectory must supply rules for building sources it contributes
Application/SW4STM32/startup_stm32f429xx.o: /Users/nimaaghli/Documents/workspace/STM32AI_QuickDraw/SW4STM32/startup_stm32f429xx.s
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Assembler'
	@echo $(PWD)
	arm-none-eabi-as -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -g -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Application/SW4STM32/syscalls.o: /Users/nimaaghli/Documents/workspace/STM32AI_QuickDraw/SW4STM32/syscalls.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed="__attribute__((__packed__))"' -DARM_MATH_CM4 '-D__FPU_PRESENT=1U' -DUSE_HAL_DRIVER -DSTM32F429xx -I"/Users/nimaaghli/Documents/workspace/STM32AI_QuickDraw/Inc" -I"/Users/nimaaghli/Documents/workspace/STM32AI_QuickDraw/Middlewares/ST/AI/AI/data" -I"/Users/nimaaghli/Documents/workspace/STM32AI_QuickDraw/Middlewares/ST/AI/AI/include" -I"/Users/nimaaghli/Documents/workspace/STM32AI_QuickDraw/Drivers/CMSIS/DSP/Include" -I"/Users/nimaaghli/Documents/workspace/STM32AI_QuickDraw/Drivers/STM32F4xx_HAL_Driver/Inc" -I"/Users/nimaaghli/Documents/workspace/STM32AI_QuickDraw/SW4STM32/STM32AI_QuickDraw/Drivers/BSP/STM32F429I-Discovery" -I"/Users/nimaaghli/Documents/workspace/STM32AI_QuickDraw/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy" -I"/Users/nimaaghli/Documents/workspace/STM32AI_QuickDraw/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"/Users/nimaaghli/Documents/workspace/STM32AI_QuickDraw/Drivers/CMSIS/Include"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


