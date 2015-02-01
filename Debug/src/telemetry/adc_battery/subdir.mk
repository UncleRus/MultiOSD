################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/telemetry/adc_battery/adc_battery.cpp 

OBJS += \
./src/telemetry/adc_battery/adc_battery.o 

CPP_DEPS += \
./src/telemetry/adc_battery/adc_battery.d 


# Each subdirectory must supply rules for building sources it contributes
src/telemetry/adc_battery/%.o: ../src/telemetry/adc_battery/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: AVR C++ Compiler'
	avr-g++ -Wall -g2 -gstabs -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -funsigned-char -funsigned-bitfields -fno-exceptions -mmcu=atmega328p -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


