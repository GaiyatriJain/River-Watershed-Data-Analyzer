CC     = gcc
CFLAGS = -Wall -Wextra -std=c11 -g

TARGET = waterwatch
SRC    = main.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

.PHONY: all clean
