################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/osd/osd.cpp \
../src/osd/panel.cpp \
../src/osd/screen.cpp 

OBJS += \
./src/osd/osd.o \
./src/osd/panel.o \
./src/osd/screen.o 

CPP_DEPS += \
./src/osd/osd.d \
./src/osd/panel.d \
./src/osd/screen.d 


# Each subdirectory must supply rules for building sources it contributes
src/osd/%.o: ../src/osd/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: AVR C++ Compiler'
	avr-g++ -Wall -g2 -gstabs -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -funsigned-char -funsigned-bitfields -fno-exceptions -mmcu=atmega328p -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


