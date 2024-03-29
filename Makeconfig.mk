FILES = robot_main utility timers usart robot_sensor drive_control state_control led_control
O_SRC = $(addprefix $(OUT_O_DIR)/, $(addsuffix .o, $(FILES)))
C_SRC = $(addsuffix .c, $(FILES))
H_SRC = $(addsuffix .h, $(FILES))
D_SRC = $(addprefix $(OUT_O_DIR)/, $(addsuffix .d, $(FILES)))
F_CPU = 16E6
DEVICE = atmega328p
PROGRAMMER_ID = arduino
PORT = /dev/ttyACM0
BAUD = 115200
OUT_O_DIR = out
TARGET_FILE = $(OUT_O_DIR)/tmpfile
DUDE_FLAGS = -p $(DEVICE) -c $(PROGRAMMER_ID) -P $(PORT) -b $(BAUD)
CFLAGS = -mmcu=${DEVICE} -Os -D F_CPU=${F_CPU} -MMD -MP
CC = avr-gcc
DOX = Doxyfile
CPPCHECK_FLAGS = --enable=style,warning