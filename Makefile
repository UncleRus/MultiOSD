BUILDDIR = build
TARGET = MultiOSD
FWDIR = firmware


all: firmware

set_tag = $(file >$(BUILDDIR)/config.mk,TAG = $1)
set_defs = $(file >>$(BUILDDIR)/config.mk,DEFS = $1)
clear = @rm -rf $(BUILDDIR)/*[!.hex]
fw_tgt = $(BUILDDIR)/$(TARGET)_$1.hex

firmware: $(call fw_tgt,uavtalk) $(call fw_tgt,uavtalk_adcrssi) $(call fw_tgt,uavtalk_adcbattery) \
	 $(call fw_tgt,uavtalk_adcbattery_adcrssi) $(call fw_tgt,mavlink) $(call fw_tgt,mavlink_adcrssi) \
	 $(call fw_tgt,ubx_adcbattery_adcrssi)

$(call fw_tgt,uavtalk):
	mkdir -p $(BUILDDIR)
	$(call set_tag,uavtalk)
	$(call set_defs,-DTELEMETRY_MODULES_UAVTALK)
	$(MAKE) -C $(FWDIR)
	$(clear)

$(call fw_tgt,uavtalk_adcrssi):
	mkdir -p $(BUILDDIR)
	$(call set_tag,uavtalk_adcrssi)
	$(call set_defs,-DTELEMETRY_MODULES_UAVTALK -DTELEMETRY_MODULES_ADC_RSSI)
	$(MAKE) -C $(FWDIR)
	$(clear)

$(call fw_tgt,uavtalk_adcbattery):
	mkdir -p $(BUILDDIR)
	$(call set_tag,uavtalk_adcbattery)
	$(call set_defs,-DTELEMETRY_MODULES_UAVTALK -DTELEMETRY_MODULES_ADC_BATTERY)
	$(MAKE) -C $(FWDIR)
	$(clear)

$(call fw_tgt,uavtalk_adcbattery_adcrssi):
	mkdir -p $(BUILDDIR)
	$(call set_tag,uavtalk_adcbattery_adcrssi)
	$(call set_defs,-DTELEMETRY_MODULES_UAVTALK -DTELEMETRY_MODULES_ADC_BATTERY -DTELEMETRY_MODULES_ADC_RSSI)
	$(MAKE) -C $(FWDIR)
	$(clear)

$(call fw_tgt,mavlink):
	mkdir -p $(BUILDDIR)
	$(call set_tag,mavlink)
	$(call set_defs,-DTELEMETRY_MODULES_MAVLINK)
	$(MAKE) -C $(FWDIR)
	$(clear)

$(call fw_tgt,mavlink_adcrssi):
	mkdir -p $(BUILDDIR)
	$(call set_tag,mavlink_adcrssi)
	$(call set_defs,-DTELEMETRY_MODULES_MAVLINK -DTELEMETRY_MODULES_ADC_RSSI)
	$(MAKE) -C $(FWDIR)
	$(clear)

$(call fw_tgt,ubx_adcbattery_adcrssi):
	mkdir -p $(BUILDDIR)
	$(call set_tag,ubx_adcbattery_adcrssi)
	$(call set_defs,-DTELEMETRY_MODULES_UBX -DTELEMETRY_MODULES_ADC_BATTERY -DTELEMETRY_MODULES_ADC_RSSI)
	$(MAKE) -C $(FWDIR)
	$(clear)

clean:
	@rm -rf $(BUILDDIR)/*

.PHONY: all clean firmware
