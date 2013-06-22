CC=gcc
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=main.c util.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=node

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

.c.o:
	$(CC) $(CFLAGS) $< -o $@


