################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../SSD1306/Src/SSD1306.c \
../SSD1306/Src/fonts.c 

OBJS += \
./SSD1306/Src/SSD1306.o \
./SSD1306/Src/fonts.o 

C_DEPS += \
./SSD1306/Src/SSD1306.d \
./SSD1306/Src/fonts.d 


# Each subdirectory must supply rules for building sources it contributes
SSD1306/Src/%.o SSD1306/Src/%.su: ../SSD1306/Src/%.c SSD1306/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I"D:/stm32/SSD1306/SSD1306_I2C/SSD1306/Inc" -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-SSD1306-2f-Src

clean-SSD1306-2f-Src:
	-$(RM) ./SSD1306/Src/SSD1306.d ./SSD1306/Src/SSD1306.o ./SSD1306/Src/SSD1306.su ./SSD1306/Src/fonts.d ./SSD1306/Src/fonts.o ./SSD1306/Src/fonts.su

.PHONY: clean-SSD1306-2f-Src

