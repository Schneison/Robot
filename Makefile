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

indent:
	# "Aufgabenblatt_3.pdf S.8"
	# "https://www.gnu.org/software/indent/manual/indent.html"
	#-kr -bad -br -brs -brf -ce -cdw -bfda -sar -i4 -lp -as -cli4
#-nbad -bap -nbc -bbo -hnl -br -brs -c33 -cd33 -ncdb -ce -ci4 -cli4 -d0 -di1 -nfc1 -i8 -ip0 -l80 -lp -npcs -nprs -npsl -sai -saf -saw -ncs -nsc -sob -nfca -cp33 -ss -ts8 -il1
#-cli4 -as -ip4 -i4 -il4 -l100 -br -cdw -ce
	indent state_control.c -kr -i4 -nut -ss -saw -psl -o state_control.out

cppcheck: $(C_SRC)
	cppcheck *.c *.h $(CPPCHECK_FLAGS)

documentation: $(C_SRC) $(H_SRC)
	doxygen $(DOX)

clean:
	-rm -f $(TARGET_FILE) *.hex $(OUT_O_DIR)/*.o

$(TARGET_FILE): $(O_SRC)
	$(CC) $(O_SRC) -o $(TARGET_FILE) $(CFLAGS)

$(TARGET_FILE).hex: $(TARGET_FILE)
	avr-objcopy -O ihex $(TARGET_FILE) $(TARGET_FILE).hex
	avrdude $(DUDE_FLAGS) -U flash:w:$(TARGET_FILE).hex:i


# target: prerequisites ; recipe
# 	recipe

# "percent" (%) sign is used for pattern matching, and it requires one in the target as well as (at least) one in the prerequisites
# "$@" means 'the target of this rule', and "$<" means 'this rule's first listed prerequisite'
$(OUT_O_DIR)/%.o: %.c %.h
	@mkdir -p $(@D)

	$(CC) $< -c $(CFLAGS)  -o $@
