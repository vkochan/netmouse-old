CC=gcc
CFLAGS=-c
LDFLAGS=-mconsole

SOURCES=netmouse.c input_handler.c mouse.c keybd.c event_pipe.c screen.c config.c

LIBS=-lws2_32 -lmswsock -ladvapi32		

OBJECTS=$(SOURCES:.c=.o)
OBJECTS+=netmouse.res

EXECUTABLE=netmouse

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

netmouse.res:
	windres netmouse.rc -O coff -o netmouse.res
	
clean:
	rm -f *.o