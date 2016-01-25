BUILDDIR = build
TARGET = MultiOSD
FWDIR = firmware

all: firmware

clear = @rm -rf $(BUILDDIR)/*[!h][!e][!x]

firmware: $(BUILDDIR)/$(TARGET)_uavtalk.hex \
          $(BUILDDIR)/$(TARGET)_uavtalk_adcrssi.hex \
          $(BUILDDIR)/$(TARGET)_uavtalk_adcbattery.hex \
          $(BUILDDIR)/$(TARGET)_uavtalk_adcbattery_adcrssi.hex \
          $(BUILDDIR)/$(TARGET)_mavlink.hex \
          $(BUILDDIR)/$(TARGET)_mavlink_adcrssi.hex \
          $(BUILDDIR)/$(TARGET)_ubx_adcbattery_adcrssi.hex

$(BUILDDIR)/$(TARGET)_uavtalk.hex:
	mkdir -p $(BUILDDIR)
	$(MAKE) TAG=uavtalk DEFS=-DTELEMETRY_MODULES_UAVTALK -C $(FWDIR)
	$(clear)

$(BUILDDIR)/$(TARGET)_uavtalk_adcrssi.hex:
	mkdir -p $(BUILDDIR)
	$(MAKE) TAG=uavtalk_adcrssi "DEFS=-DTELEMETRY_MODULES_UAVTALK -DTELEMETRY_MODULES_ADC_RSSI" -C $(FWDIR)
	$(clear)

$(BUILDDIR)/$(TARGET)_uavtalk_adcbattery.hex:
	mkdir -p $(BUILDDIR)
	$(MAKE) TAG=uavtalk_adcbattery "DEFS=-DTELEMETRY_MODULES_UAVTALK -DTELEMETRY_MODULES_ADC_BATTERY" -C $(FWDIR)
	$(clear)

$(BUILDDIR)/$(TARGET)_uavtalk_adcbattery_adcrssi.hex:
	mkdir -p $(BUILDDIR)
	$(MAKE) TAG=uavtalk_adcbattery_adcrssi "DEFS=-DTELEMETRY_MODULES_UAVTALK -DTELEMETRY_MODULES_ADC_BATTERY -DTELEMETRY_MODULES_ADC_RSSI" -C $(FWDIR)
	$(clear)

$(BUILDDIR)/$(TARGET)_mavlink.hex:
	mkdir -p $(BUILDDIR)
	$(MAKE) TAG=mavlink DEFS=-DTELEMETRY_MODULES_MAVLINK -C $(FWDIR)
	$(clear)

$(BUILDDIR)/$(TARGET)_mavlink_adcrssi.hex:
	mkdir -p $(BUILDDIR)
	$(MAKE) TAG=mavlink_adcrssi "DEFS=-DTELEMETRY_MODULES_MAVLINK -DTELEMETRY_MODULES_ADC_RSSI" -C $(FWDIR)
	$(clear)

$(BUILDDIR)/$(TARGET)_ubx_adcbattery_adcrssi.hex:
	mkdir -p $(BUILDDIR)
	$(MAKE) TAG=ubx_adcbattery_adcrssi "DEFS=-DTELEMETRY_MODULES_UBX -DTELEMETRY_MODULES_ADC_BATTERY -DTELEMETRY_MODULES_ADC_RSSI" -C $(FWDIR)
	$(clear)

clean:
	@rm -rf $(BUILDDIR)/*

.PHONY: all clean firmware
