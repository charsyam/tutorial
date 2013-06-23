CC=gcc
CFLAGS=-c -Wall -I/usr/include/libxml2
LDFLAGS=
LIBS = -lxml2
SERVER_SOURCES=server.c util.c
SERVER_OBJECTS=$(SERVER_SOURCES:.c=.o)
CLIENT_SOURCES=client.c util.c
CLIENT_OBJECTS=$(CLIENT_SOURCES:.c=.o)
SERVER=server
CLIENT=client

all: $(SOURCES) $(SERVER) $(CLIENT)
	
$(SERVER): $(SERVER_OBJECTS)
	$(CC) $(LDFLAGS) $(SERVER_OBJECTS) -o $@

$(CLIENT): $(CLIENT_OBJECTS)
	$(CC) $(LDFLAGS) $(CLIENT_OBJECTS) -o $@ $(LIBS)

clean:
	rm -f $(SERVER_OBJECTS) $(CLIENT_OBJECTS) $(SERVER) $(CLIENT)

.c.o:
	$(CC) $(CFLAGS) $< -o $@


