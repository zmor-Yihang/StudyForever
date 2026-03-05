CC = gcc
CFLAGS = -Wall -Wextra -O2
TARGETS = poll_reader poll_writer

all: $(TARGETS)

poll_reader: poll_reader.c
	$(CC) $(CFLAGS) -o $@ $<

poll_writer: poll_writer.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS)

.PHONY: all clean