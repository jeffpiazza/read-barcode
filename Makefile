CC      := gcc
CFLAGS  := -Wall -Wextra -O2
LDFLAGS := -lrt

.PHONY: all clean

all: clean barcode

clean:
	rm -f barcode *.o

install:
	cp barcode /usr/local/bin

%.o: %.c
	$(CC) $(CFLAGS) -c $^

barcode: barcode.o
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o barcode
