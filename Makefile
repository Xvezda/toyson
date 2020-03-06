.SUFFIXES: .c

CC = gcc
CCFLAGS = -g -O0
TARGET = toyson_test
SRC_FILES = $(wildcard *.c)
OBJ_FILES = $(patsubst %.c, %.o, $(SRC_FILES))

all: $(TARGET)

$(TARGET): $(OBJ_FILES)
	$(CC) $(CCFLAGS) $(OBJ_FILES) -o $@

%.o: %.c
	$(CC) $(CCFLAGS) -c $< -o $@

clean:
	rm -f *.o $(TARGET)
