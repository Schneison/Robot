C_SRC = drive_test.c iesmotors.c iesusart.c robot_sensor.c
F_CPU = 16E6
DEVICE = atmega328p
PROGRAMMER_ID = arduino
PORT = /dev/ttyACM0
BAUD_RATE = 115200
TARGET_FILE = tmpfile

CFLAGS = -o ${FILE_NAME} -mmcu=${DEVICE} -Os -D F_CPU=${F_CPU}
CC = avr-gcc

# all targets that don't correspond to files
.PHONY : compile clean

all: compile

compile:
	$(CC) $(C_SRC) $(CFLAGS)
	avr-objcopy -O ihex $(TARGET_FILE) $(TARGET_FILE).hex
	avrdude -p $(DEVICE) -c $(PROGRAMMER_ID) -P $(PORT) -b $(BAUD_RATE) -U flash:w:$(TARGET_FILE).hex:i

clean:
	-rm -f *.o $(TARGET_FILE) *.hex

%.o: %.c %.h
	$(CC) $< -c $(CFLAGS)
