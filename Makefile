include Makeconfig.mk

# all targets that don't correspond to files
.PHONY: info force list-headers help cppcheck compile flash documentation link clean

all: compile link flash documentation
	@echo Done.

force: clean all

help:
	@echo "Some useful make targets:"
	@echo " make all          	- Build entire project and documentation"
	@echo " make compile      	- Compiles the .c and .h files"
	@echo " make link         	- Links the .c and .h files to .o files"
	@echo " make flash        	- Flashes the project to the board via serial"
	@echo " make force        	- Force rebuild of entire project and documentation (clean first)"
	@echo " make clean        	- Remove all build output"
	@echo " make cppcheck     	- Static code analysis tool for the C"
	@echo " make indent       	- Applies style conventions to current .c and .h files"
	@echo " make documentation	- Create doxygen documentation for this project"
	@echo " make try_connect    - Tries to establish a connection with a robot via bluetooth"
	@echo " make try_disconnect	- Tries to disconnect the connection to the robot if any is existent"
	@echo ""


info:
	@echo "[*] Output dir:      ${OUT_O_DIR}   "
	@echo "[*] Sources:         ${C_SRC}       "
	@echo "[*] Headers:         ${H_SRC}       "
	@echo "[*] Objects:         ${O_SRC}       "
	@echo "[*] Devise:          ${DEVICE}      "
	@echo "[*] Baud:        	${BAUD}        "

compile: $(C_SRC) $(H_SRC)

link: $(TARGET_FILE)

flash: $(TARGET_FILE).hex
	avrdude $(DUDE_FLAGS) -U flash:w:$(TARGET_FILE).hex:i

indent: $(C_SRC) $(H_SRC)
	@mkdir -p indent
	for file in $(C_SRC); do \
  		indent $$file -brf -kr -i4 -nut -o indent/$$file;\
  	done

	for file in $(H_SRC); do \
		indent $$file -brf -kr -i4 -nut -o indent/$$file;\
	done

cppcheck: $(C_SRC)
	cppcheck *.c *.h $(CPPCHECK_FLAGS)

documentation: $(C_SRC) $(H_SRC)
	doxygen $(DOX)

clean:
	-rm -f $(TARGET_FILE) *.hex $(OUT_O_DIR)/*.o $(OUT_O_DIR)/*.d

try_connect:
	BotBtSerial connect

try_disconnect:
	BotBtSerial disconnect

$(TARGET_FILE): $(O_SRC)
	$(CC) $(O_SRC) -o $(TARGET_FILE) $(CFLAGS)

$(TARGET_FILE).hex: $(TARGET_FILE)
	avr-objcopy -O ihex $(TARGET_FILE) $(TARGET_FILE).hex

-include $(D_SRC)
$(OUT_O_DIR)/%.o: %.c %.h
	@mkdir -p $(@D)

	$(CC) $< -c $(CFLAGS)  -o $@
