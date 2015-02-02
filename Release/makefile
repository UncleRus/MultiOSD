################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/telemetry/uavtalk/subdir.mk
-include src/telemetry/adc_battery/subdir.mk
-include src/telemetry/subdir.mk
-include src/osd/subdir.mk
-include src/lib/uart/subdir.mk
-include src/lib/timer/subdir.mk
-include src/lib/spi/subdir.mk
-include src/lib/max7456/subdir.mk
-include src/lib/console/subdir.mk
-include src/lib/adc/subdir.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
LSS += \
min_raw_osd.lss \

FLASH_IMAGE += \
min_raw_osd.hex \

EEPROM_IMAGE += \
min_raw_osd.eep \

SIZEDUMMY += \
sizedummy \


# All Target
all: min_raw_osd.elf secondary-outputs

# Tool invocations
min_raw_osd.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C++ Linker'
	avr-g++ -Wl,-Map,min_raw_osd.map,--cref -mrelax -Wl,--gc-sections -Wl,-u,vfprintf -lprintf_flt -lm -mmcu=atmega328p -o "min_raw_osd.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

min_raw_osd.lss: min_raw_osd.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S min_raw_osd.elf  >"min_raw_osd.lss"
	@echo 'Finished building: $@'
	@echo ' '

min_raw_osd.hex: min_raw_osd.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex min_raw_osd.elf  "min_raw_osd.hex"
	@echo 'Finished building: $@'
	@echo ' '

min_raw_osd.eep: min_raw_osd.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex min_raw_osd.elf  "min_raw_osd.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: min_raw_osd.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega328p min_raw_osd.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EEPROM_IMAGE)$(FLASH_IMAGE)$(ELFS)$(LSS)$(S_DEPS)$(CXX_DEPS)$(S_UPPER_DEPS)$(C++_DEPS)$(ASM_DEPS)$(CC_DEPS)$(CPP_DEPS)$(C_UPPER_DEPS)$(SIZEDUMMY) min_raw_osd.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
