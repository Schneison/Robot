#!/bin/bash
echo avr-gcc drive_test.c iesmotors.c iesusart.c robot_sensor.c -o tmpfile -mmcu=atmega328p -Os -D F_CPU=16E6
avr-gcc drive_test.c iesmotors.c iesusart.c robot_sensor.c -o tmpfile -mmcu=atmega328p -Os -D F_CPU=16E6

echo avr-objcopy -O ihex tmpfile tmpfile.hex
avr-objcopy -O ihex tmpfile tmpfile.hex

echo avrdude -p atmega328p -c arduino -P /dev/ttyACM0 -b 115200 -U flash:w:tmpfile.hex:i
avrdude -p atmega328p -c arduino -P /dev/ttyACM0 -b 115200 -U flash:w:tmpfile.hex:i

#echo rm ./tmpfile ./tmpfile.hex
#rm ./tmpfile ./tmpfile.hex
