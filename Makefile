CC=gcc
OPTIMIZE=-Ofast
CFLAGS=-Wall -Wstrict-prototypes -Werror $(OPTIMIZE)
LIBS=
RM=rm -f

OBJECTS=main.o prometheus.o utils.o
PROG= main

all: $(PROG)

$(PROG): $(OBJECTS)
	$(CC) -o $(PROG) $(OBJECTS) $(LIBS)

clean:
	$(RM) $(OBJECTS) main

