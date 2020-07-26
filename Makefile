CC=gcc
OPTIMIZE=-Ofast
CFLAGS=-Wall -Wstrict-prototypes -Werror $(OPTIMIZE)
LIBS=
RM=rm -f

OBJECTS=main.o prometheus.o utils.o server.o
PROG= main

all: $(PROG)

$(PROG): $(OBJECTS)
	$(CC) -o $(PROG) $(OBJECTS) $(LIBS)

test: $(PROG) 
	rm -rf /var/tmp/p.tar.gz /var/tmp/prom* || echo "No previous prometheus installation found"
	cd /var/tmp ; wget https://github.com/prometheus/prometheus/releases/download/v2.20.0/prometheus-2.20.0.linux-amd64.tar.gz -O p.tar.gz
	cd /var/tmp ; tar xzvf p.tar.gz
	./$(PROG) | /var/tmp/prome*/promtool check metrics

clean:
	$(RM) $(OBJECTS) main

