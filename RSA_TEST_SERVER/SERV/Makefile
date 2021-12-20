CC = gcc
SRC_DIR = ./src
OBJ_DIR = ./obj
TARGET = merge

SRCS = $(notdir $(wildcard $(SRC_DIR)/*.c))
OBJS = $(SRCS:.c=.o)

OBJECTS = $(patsubst %.o,$(OBJ_DIR)/%.o,$(OBJS))

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.c
	$(CC) -c -o $@ $^ -g

$(TARGET) : $(OBJECTS)
	$(CC) -o $(TARGET) $(OBJECTS) -g

clean:
	rm -f $(OBJECTS) $(TARGET) varset/Result_set.txt core*
