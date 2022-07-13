include Makeconfig.mk

# all targets that don't correspond to files
.PHONY: flash documentation link clean

all: link flash documentation

link: $(TARGET_FILE)

flash: $(TARGET_FILE).hex

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
