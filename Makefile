SOURCE_DIR := src
BUILD_DIR := build
PROJECT_NAME := sdCardSave

include $(N64_INST)/include/n64.mk

SRCS_C := $(SOURCE_DIR)/main.c
OBJS := $(BUILD_DIR)/main.o

all: $(PROJECT_NAME).z64

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "    [CC] $<"
	$(CC) $(CFLAGS) -c $< -o $@

$(PROJECT_NAME).z64: N64_ROM_TITLE="$(PROJECT_NAME)"
$(BUILD_DIR)/$(PROJECT_NAME).elf: $(OBJS)

clean:
	rm -rf $(BUILD_DIR) $(PROJECT_NAME).z64

.PHONY: all clean