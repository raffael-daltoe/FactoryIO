################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../cmsis/device/src/startup_stm32f072xb.s 

C_SRCS += \
../cmsis/device/src/system_stm32f0xx.c 

OBJS += \
./cmsis/device/src/startup_stm32f072xb.o \
./cmsis/device/src/system_stm32f0xx.o 

S_DEPS += \
./cmsis/device/src/startup_stm32f072xb.d 

C_DEPS += \
./cmsis/device/src/system_stm32f0xx.d 


# Each subdirectory must supply rules for building sources it contributes
cmsis/device/src/%.o: ../cmsis/device/src/%.s cmsis/device/src/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m0 -g3 -c -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@" "$<"
cmsis/device/src/%.o cmsis/device/src/%.su cmsis/device/src/%.cyclo: ../cmsis/device/src/%.c cmsis/device/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=gnu11 -g3 -DSTM32F0xB -c -I"/home/raffael/STM/workspace_tuto/TP3/app/inc" -I"/home/raffael/STM/workspace_tuto/TP3/TraceRecorder/Include" -I"/home/raffael/STM/workspace_tuto/TP3/FreeRTOS/include" -I"/home/raffael/STM/workspace_tuto/TP3/FreeRTOS/portable/GCC/ARM_CM0" -I"/home/raffael/STM/workspace_tuto/TP3/bsp/inc" -I"/home/raffael/STM/workspace_tuto/TP3/cmsis/core" -I"/home/raffael/STM/workspace_tuto/TP3/cmsis/device/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-cmsis-2f-device-2f-src

clean-cmsis-2f-device-2f-src:
	-$(RM) ./cmsis/device/src/startup_stm32f072xb.d ./cmsis/device/src/startup_stm32f072xb.o ./cmsis/device/src/system_stm32f0xx.cyclo ./cmsis/device/src/system_stm32f0xx.d ./cmsis/device/src/system_stm32f0xx.o ./cmsis/device/src/system_stm32f0xx.su

.PHONY: clean-cmsis-2f-device-2f-src

