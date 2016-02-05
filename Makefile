
SOURCES = $(wildcard *.c)
INCLUDES = $(wildcard *.h)
OBJECTS = $(SOURCES:.c=.o)

CFLAGS = $(shell pkg-config --cflags glib-2.0 gtk+-2.0) -ggdb -Wall
LIBS   = $(shell pkg-config --libs glib-2.0 gtk+-2.0) -lm

all: torquesym

torquesym: $(OBJECTS)
	$(CC) -o torquesym $(LIBS) $(OBJECTS)

%.o: %.c $(INCLUDES)
	$(CC) -c $< $(CFLAGS)

clean:
	rm -f torquesym *.o *~

