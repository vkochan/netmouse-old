CC=gcc
CFLAGS=-c -iquote .\..
LDFLAGS=-r -Wl -nostdlib
RESLAGS=-O coff

SOURCES=taskbar.c logger.c	

OBJECTS=$(SOURCES:.c=.o)

MODULE=gui.o

.PHONY: clean

all: clean $(MODULE)

$(MODULE): $(SOURCES) $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@
	
.c.o:
	$(CC) $(CFLAGS) $< -o $@
	
clean:
	clean.sh