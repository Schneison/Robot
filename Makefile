C_SRC = drive_test.c iesmotors.c iesusart.c robot_sensor.c robot_controller.c
H_SRC = iesmotors.h iesusart.h robot_sensor.h robot_controller.h
O_SRC = drive_test.o iesmotors.o iesusart.o robot_sensor.o robot_controller.o
F_CPU = 16E6
DEVICE = atmega328p
PROGRAMMER_ID = arduino
PORT = /dev/ttyACM0
BAUD = 115200
TARGET_FILE = tmpfile

DUDE_FLAGS = -p $(DEVICE) -c $(PROGRAMMER_ID) -P $(PORT) -b $(BAUD)
CFLAGS = -mmcu=${DEVICE} -Os -D F_CPU=${F_CPU}
CC = avr-gcc

# all targets that don't correspond to files
.PHONY: compile flash documentation link clean

all: compile link flash documentation

link: $(TARGET_FILE)

flash: $(TARGET_FILE).hex

compile: $(C_SRC) $(H_SRC)

documentation: $(C_SRC) $(H_SRC)
	doxygen Doxyfile

clean:
	-rm -f *.o $(TARGET_FILE) *.hex

$(TARGET_FILE): $(O_SRC)
	$(CC) $(O_SRC) -o $(TARGET_FILE) $(CFLAGS)

$(TARGET_FILE).hex: $(TARGET_FILE)
	avr-objcopy -O ihex $(TARGET_FILE) $(TARGET_FILE).hex
	avrdude $(DUDE_FLAGS) -U flash:w:$(TARGET_FILE).hex:i

drive_test.o: drive_test.c
	$(CC) drive_test.c -c $(CFLAGS)

drive_controller.c:

drive_controller.o: robot_controller.c
	$(CC) robot_controller.c -c $(CFLAGS)

# target: prerequisites ; recipe
# 	recipe

# "percent" (%) sign is used for pattern matching, and it requires one in the target as well as (at least) one in the prerequisites
# "$@" means 'the target of this rule', and "$<" means 'this rule's first listed prerequisite'
%.o: %.c %.h
	$(CC) $< -c $(CFLAGS)
