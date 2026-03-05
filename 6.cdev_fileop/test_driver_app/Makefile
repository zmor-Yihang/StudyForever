CC := gcc
CFLAGS := -Wall -Wextra -std=c99 -O2

TARGET := error_handling_test
SOURCE := error_handling_test.c

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGET)

install: all
	sudo cp $(TARGET) /usr/local/bin/

uninstall:
	sudo rm -f /usr/local/bin/$(TARGET)