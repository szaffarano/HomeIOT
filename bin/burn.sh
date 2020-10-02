#!/usr/bin/env bash 

TARGET=atmega328_pro8
AVR_FREQ=8000000L
BAUD_RATE=9600
MCU_TARGET=atmega328
LED_START_FLASHES=5

AVRDUDE="avrdude"

which "$AVRDUDE" > /dev/null || die "$AVRDUDE: not found"

function die {
  echo "$1"
  exit 1
}

BASE="$(readlink -f "$(dirname "$0")")"
OPTIBOOT="$(readlink -f "$BASE"/../optiboot/optiboot/bootloaders/optiboot/)"

pushd "$OPTIBOOT" > /dev/null || die "Unable to enter to optiboot: $OPTIBOOT"

make \
  clean \
  "$TARGET" \
  AVR_FREQ="$AVR_FREQ" \
  BAUD_RATE="$BAUD_RATE" \
  MCU_TARGET="$MCU_TARGET" \
  LED_START_FLASHES="$LED_START_FLASHES"

BOOTLOADER="$(find . -maxdepth 1 -type f -name '*.hex')"

[ -n "$BOOTLOADER" ] || die "Error building bootloader"

echo "######################"
echo "Burning fuses...      "
echo "######################"

$AVRDUDE \
    -c usbasp \
    -p $MCU_TARGET \
    -U lfuse:w:"$LFUSE":m \
    -U hfuse:w:"$LFUSE":m \
    -U efuse:w:"$LFUSE":m \
    -U lock:w:"$LOCK":m

echo "########################################################################"
echo "Burning bootloader:  file: $BOOTLOADER"
echo "########################################################################"

$AVRDUDE \
    -p $MCU_TARGET \
    -cusbasp \
    -Pusb \
    -Uflash:w:"$BOOTLOADER":i \
    -Ulock:w:"$LOCK":m

make clean

popd > /dev/null || return
