TARGET := program

CC := gcc
CFLAGS := -g -Wall -Wextra -std=c99 -I.
CFLAGS += -I/usr/include/freetype2 -I/usr/include/libpng16
LLIBRARIES := -lSDL3 -lSDL3_ttf -lfreetype


BUILD_DIR := build
SRC_DIRS := $(shell find . -type d)
SRCS := $(foreach dir, $(SRC_DIRS), $(wildcard $(dir)/*.c))
OBJS := $(patsubst ./%, $(BUILD_DIR)/%, $(SRCS:.c=.o))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LLIBRARIES) $(OBJS) -o $@

$(BUILD_DIR)/%.o: ./%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

