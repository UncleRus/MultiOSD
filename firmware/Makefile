BUILD_DIR = ../build

TAG ?= mavlink
DEFS ?= -DTELEMETRY_MODULES_MAVLINK

TARGET = MultiOSD_$(TAG)
SOURCE_DIR = 
TARGET_MCU = atmega328p
F_CPU = 16000000

# It can be "float", "min" or empty
STDIO_VPRINTF = float
LIBS =

#######################################################################

SRCS := $(shell find $(SOURCE_DIR) -name '*.cpp')
HDRS := $(shell find $(SOURCE_DIR) -name '*.h')
OBJS := $(addprefix $(BUILD_DIR)/,$(SRCS:%.cpp=%.o))
DEPS := $(addprefix $(BUILD_DIR)/,$(SRCS:%.cpp=%.d))

ifeq ($(strip $(STDIO_VPRINTF)),float)
LIBS += -Wl,-u,vfprintf -lprintf_flt -lm
else ifeq ($(strip $(STDIO_VPRINTF)),min)
LIBS += -Wl,-u,vfprintf -lprintf_min
endif

ELF = $(BUILD_DIR)/$(TARGET).elf
LSS = $(BUILD_DIR)/$(TARGET).lss
HEX = $(BUILD_DIR)/$(TARGET).hex
MAP = $(BUILD_DIR)/$(TARGET).map

CXX = avr-g++
CXXFLAGS = -Wall -g2 -gstabs -Os -fpack-struct -fshort-enums -ffunction-sections \
	-fdata-sections -funsigned-char -funsigned-bitfields -fno-exceptions -mmcu=$(TARGET_MCU) \
	-MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c

LD = avr-g++
LDFLAGS = -Wl,-Map,$(MAP),--cref -mrelax -Wl,--gc-sections $(LIBS) -mmcu=$(TARGET_MCU)

OBJDUMP = -avr-objdump
OBJDUMP_FLAGS = -h -S

OBJCOPY = -avr-objcopy
OBJCOPY_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature
OBJCOPY_FMT = ihex

AVRSIZE = -avr-size
AVRSIZE_FLAGS = --format=avr --mcu=$(TARGET_MCU)  

all: $(ELF) secondary-outputs

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) -DF_CPU=$(F_CPU)UL $(DEFS) $(CXXFLAGS) -o "$@" $<

$(ELF): $(OBJS)
	@echo 'Building target: $<'
	$(LD) $(LDFLAGS) -o "$@" $(OBJS)

$(LSS): $(ELF)
	@echo 'AVR Extended Listing'
	$(OBJDUMP) $(OBJDUMP_FLAGS) "$(ELF)" > "$(LSS)"

$(HEX): $(ELF)
	@echo 'Create Flash image ($(OBJCOPY_FMT) format)'
	$(OBJCOPY) $(OBJCOPY_FLAGS) -O $(OBJCOPY_FMT) "$(ELF)" "$(HEX)"

sizedummy: $(ELF)
	@echo 'Print Size'
	$(AVRSIZE) $(AVRSIZE_FLAGS) "$(ELF)"

secondary-outputs: $(LSS) $(HEX) sizedummy

clean:
	rm -f $(OBJS) $(LSS) $(HEX) $(MAP) $(ELF) $(DEPS)

.PHONY: all clean dependents sizedummy secondary-outputs 
.SECONDARY:
