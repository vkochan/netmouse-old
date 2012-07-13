CC=gcc
CFLAGS=-c
LDFLAGS=-mconsole

SOURCES=netmouse.c service.c input_handler.c mouse.c keybd.c event_pipe.c event_receiver.c event_sender.c config.c

LIBS=-lws2_32 -lmswsock -ladvapi32		
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=netmouse

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o