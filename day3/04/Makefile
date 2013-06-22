CC=gcc
CFLAGS=-c -Wall -I/usr/include/libxml2
LDFLAGS= 
SOURCES=xml.c
LIBS= -lxml2
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=xml

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@ $(LIBS)

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

.c.o:
	$(CC) $(CFLAGS) $< -o $@
