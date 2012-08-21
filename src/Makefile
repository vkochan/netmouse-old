CC=gcc
CFLAGS=-c
#LDFLAGS=-mwindows
LDFLAGS=-mconsole -L ..\libs

SOURCES=netmouse.c input_handler.c mouse.c keybd.c net_pipe.c screen.c config.c log.c winapi.c list.c slab.c

LIBS=-lws2_32 -lmswsock -ladvapi32 -lpowrprof	

OBJECTS=$(SOURCES:.c=.o)
OBJECTS+=netmouse.res

GUI=gui\gui.o

EXECUTABLE=netmouse

.PHONY: gui

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

netmouse.res:
	windres netmouse.rc -O coff -o netmouse.res

gui:
	$(MAKE) -C .\gui
	
clean:
	rm -f *.o