CC = gcc
CFLAGS = -Wall -Wextra -O2
TARGETS = signal_reader signal_writer

all: $(TARGETS)

signal_reader: signal_reader.c
	$(CC) $(CFLAGS) -o $@ $<

signal_writer: signal_writer.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS)

.PHONY: all clean