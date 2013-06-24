CC=gcc
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=http.c 
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=http

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

.c.o:
	$(CC) $(CFLAGS) $< -o $@


