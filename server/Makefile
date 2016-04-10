TARGET = twitterTrend_server

EXEC_DIR = ../
INC_DIR = include
OBJ_DIR = src
LIBS = -lm
CC = gcc
CFLAGS = -g -Wall

.PHONY: default all clean

default: $(TARGET)
all: default

OBJECTS = $(patsubst %.c, %.o, $(wildcard $(OBJ_DIR)/*.c))
HEADERS = $(wildcard $(INC_DIR)/*.h)

%.o: %.c $(HEADERS)
		$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
		$(CC) $(OBJECTS) -lpthread -Wall $(LIBS) -o $(EXEC_DIR)$@

clean:
		-rm -rf $(OBJ_DIR)/*.o $(OBJ_DIR)/*dSYM ../*.result
		-rm -f $(EXEC_DIR)$(TARGET)
