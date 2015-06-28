#!/bin/bash

VER=0005

function clr ()
{
	rm -f build/MultiOSD_${1}_$VER.lss
	rm -f build/MultiOSD_${1}_$VER.elf
	rm -f build/MultiOSD_${1}_$VER.map
}

function build ()
{
	echo "TAG = ${1}_$VER" > config.mk
	echo "DEFS = $2" >> config.mk
	make clean
	make
	clr $1
}

mkdir -p build

build mavlink -DTELEMETRY_MODULES_MAVLINK
build uavtalk -DTELEMETRY_MODULES_UAVTALK
build uavtalk_adcbattery "-DTELEMETRY_MODULES_UAVTALK -DTELEMETRY_MODULES_ADC_BATTERY"

rm -rf build/firmware
