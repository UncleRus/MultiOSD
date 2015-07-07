BUILD_DIR = build
TARGET = MultiOSD
FW_DIR = firmware


all: firmware

set_tag = $(file >$(BUILD_DIR)/config.mk,TAG = $1)
set_defs = $(file >>$(BUILD_DIR)/config.mk,DEFS = $1)
clear = @rm -rf $(BUILD_DIR)/*[!.hex]
fw_tgt = $(BUILD_DIR)/$(TARGET)_$1.hex

firmware: $(call fw_tgt,uavtalk) $(call fw_tgt,uavtalk_adcrssi) $(call fw_tgt,uavtalk_adcbattery) $(call fw_tgt,uavtalk_adcbattery_adcrssi) $(call fw_tgt,mavlink)

$(call fw_tgt,uavtalk):
	$(call set_tag,uavtalk)
	$(call set_defs,-DTELEMETRY_MODULES_UAVTALK)
	$(MAKE) -C $(FW_DIR)
	$(clear)

$(call fw_tgt,uavtalk_adcrssi):
	$(call set_tag,uavtalk_adcrssi)
	$(call set_defs,-DTELEMETRY_MODULES_UAVTALK -DTELEMETRY_MODULES_ADC_RSSI)
	$(MAKE) -C $(FW_DIR)
	$(clear)

$(call fw_tgt,uavtalk_adcbattery):
	$(call set_tag,uavtalk_adcbattery)
	$(call set_defs,-DTELEMETRY_MODULES_UAVTALK -DTELEMETRY_MODULES_ADC_BATTERY)
	$(MAKE) -C $(FW_DIR)
	$(clear)

$(call fw_tgt,uavtalk_adcbattery_adcrssi):
	$(call set_tag,uavtalk_adcbattery_adcrssi)
	$(call set_defs,-DTELEMETRY_MODULES_UAVTALK -DTELEMETRY_MODULES_ADC_BATTERY -DTELEMETRY_MODULES_ADC_RSSI)
	$(MAKE) -C $(FW_DIR)
	$(clear)

$(call fw_tgt,mavlink):
	$(call set_tag,mavlink)
	$(call set_defs,-DTELEMETRY_MODULES_MAVLINK)
	$(MAKE) -C $(FW_DIR)
	$(clear)

$(call fw_tgt,msp):
	$(call set_tag,mavlink)
	$(call set_defs,-DTELEMETRY_MODULES_MSP)
	$(MAKE) -C $(FW_DIR)
	$(clear)

clean:
	@rm -rf $(BUILD_DIR)/*

.PHONY: all clean firmware
