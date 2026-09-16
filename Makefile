# project
TARGET    = blink-fsm
BUILD_DIR = build
BIN_DIR   = bin

# libopencm3
OPENCM3_DIR = libopencm3
DEVICE = stm32f103c8t6

# tools
PREFIX  = arm-none-eabi-
CC      = $(PREFIX)gcc
OBJCOPY = $(PREFIX)objcopy
SIZE    = $(PREFIX)size

# genlink
include $(OPENCM3_DIR)/mk/genlink-config.mk

# flags
OPT   = -O0 -g3
WARN  = -Wall -Wextra -Wpedantic
STD   = -std=c11
INCLUDES = -Isrc

# link
LDFLAGS += $(ARCH_FLAGS) -T$(LDSCRIPT) -nostartfiles
LDFLAGS += -Wl,--gc-sections -Wl,-Map=$(BUILD_DIR)/$(TARGET).map
LDLIBS  += -Wl,--start-group -lc -lgcc -lnosys -Wl,--end-group

# sources/objects
SRCS = $(wildcard src/*.c)
OBJS = $(patsubst src/%.c,$(BUILD_DIR)/%.o,$(SRCS))

# targets
.PHONY: all clean flash size

all: $(BIN_DIR)/$(TARGET).elf $(BIN_DIR)/$(TARGET).bin

# rules
$(BUILD_DIR)/%.o: src/%.c | $(BUILD_DIR)
	$(CC) $(ARCH_FLAGS) $(OPT) $(WARN) $(STD) $(CPPFLAGS) $(INCLUDES) -c $< -o $@

$(BIN_DIR)/$(TARGET).elf: $(OBJS) $(LDSCRIPT) | $(BIN_DIR)
	$(CC) $(LDFLAGS) $(OBJS) $(LDLIBS) -o $@
	$(SIZE) $@

$(BIN_DIR)/$(TARGET).bin: $(BIN_DIR)/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@

# dir
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# flash
flash: $(BIN_DIR)/$(TARGET).bin
	st-flash write $< 0x8000000

size: $(BIN_DIR)/$(TARGET).elf
	$(SIZE) $<

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR) generated.$(DEVICE).ld

# genlink rules
include $(OPENCM3_DIR)/mk/genlink-rules.mk