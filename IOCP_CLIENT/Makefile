CC = gcc
SRC_DIR = ./src
OBJ_DIR = ./obj
TARGET = iocp_client

SRCS = $(notdir $(wildcard $(SRC_DIR)/*.c))
OBJS = $(SRCS:.c=.o)

OBJECTS = $(patsubst %.o,$(OBJ_DIR)/%.o,$(OBJS))

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.c
    $(CC) -c -o $@ $^ -lssl -lcrypto -g

$(TARGET) : $(OBJECTS)
    $(CC) -o $(TARGET) $(OBJECTS) -lssl -lcrypto -g


clean :
    rm -f $(OBJECTS) $(TARGET)

