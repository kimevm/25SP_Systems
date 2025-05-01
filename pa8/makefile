CC      := gcc
CFLAGS  := -std=gnu99 -Wall -Werror -pthread -O0
LDFLAGS := -lm -lpthread        
SRC     := main.c matrix_mult.c
OBJ     := $(SRC:.c=.o)
TARGET  := matrix_mult

.PHONY: all clean

all: $(TARGET)
$(TARGET): $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) -o $@
%.o: %.c matrix_mult.h
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -f $(OBJ) $(TARGET)
