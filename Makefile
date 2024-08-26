# Put your stlink folder here so make burn will work.
STLINK=/home/emir/msut/STM32F407/stlink

# Binaries will be generated with this name (.elf, .bin, .hex, etc)
PROJ_NAME=main

# Put your STM32F4 library code directory here
STM_COMMON=/home/emir/msut/STM32F407

# Source and header directories
SRC_DIR = src
INC_DIR = inc

# Build directories
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
ASM_DIR = $(BUILD_DIR)/asm
BIN_DIR = $(BUILD_DIR)/bin

# Source files
SRCS = $(SRC_DIR)/main.c $(SRC_DIR)/system_stm32f4xx.c $(SRC_DIR)/usart.c \
       $(SRC_DIR)/delay.c $(SRC_DIR)/misc.c $(SRC_DIR)/bayer.c

# Add startup file to build
SRCS += $(STM_COMMON)/Libraries/CMSIS/ST/STM32F4xx/Source/Templates/TrueSTUDIO/startup_stm32f4xx.s 

# Object files
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
OBJS := $(OBJS:$(STM_COMMON)/Libraries/CMSIS/ST/STM32F4xx/Source/Templates/TrueSTUDIO/%.s=$(OBJ_DIR)/%.o)

# Toolchain and utilities
GNUGCC = $(STM_COMMON)/gcc-arm-none-eabi/bin
CC = $(GNUGCC)/arm-none-eabi-gcc
OBJCOPY = $(GNUGCC)/arm-none-eabi-objcopy
SIZE = $(GNUGCC)/arm-none-eabi-size

# Compiler flags
CFLAGS  = -g -O2 -Wfatal-errors -Tstm32_flash.ld 
CFLAGS += -mlittle-endian -mthumb -mcpu=cortex-m4 -mthumb-interwork
CFLAGS += -DSL_FULL -DDEBUG
CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16 -fsingle-precision-constant
CFLAGS += -I$(INC_DIR)
CFLAGS += -I$(STM_COMMON)/Libraries/CMSIS/Include 
CFLAGS += -I$(STM_COMMON)/Libraries/CMSIS/ST/STM32F4xx/Include

# Targets
.PHONY: all clean upload

all: $(BIN_DIR)/$(PROJ_NAME).elf

$(BIN_DIR)/$(PROJ_NAME).elf: $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -lm -lc -lnosys -o $@ 
	$(OBJCOPY) -O ihex $@ $(BIN_DIR)/$(PROJ_NAME).hex
	$(OBJCOPY) -O binary $@ $(BIN_DIR)/$(PROJ_NAME).bin
	$(SIZE) -B  $@
	ls -l $(BIN_DIR)/$(PROJ_NAME).bin

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(STM_COMMON)/Libraries/CMSIS/ST/STM32F4xx/Source/Templates/TrueSTUDIO/%.s
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(ASM_DIR)
	$(CC) $(CFLAGS) -c $< -o $@
	$(CC) $(CFLAGS) -S $< -o $(ASM_DIR)/$(notdir $<)

clean:
	rm -rf $(BUILD_DIR)
	ls

upload: all
	$(STLINK)/st-flash write $(BIN_DIR)/$(PROJ_NAME).bin 0x08000000

