CFLAGS = -Wall -Wextra
CLFLAGS = 
LDFLAGS = -lcrypto -lssl

all: CFLAGS += -O3 -mavx -march=native
all: aes

debug: CFLAGS += -DDEBUG -g -pg
debug: CLFLAGS += -pg
debug: aes

debug2: CFLAGS += -DDEBUG2
debug2: debug

prof: CFLAGS += -pg
prof: CLFLAGS += -pg
prof: aes

time: CFLAGS += -DTIME
time: clean aes

aes.o: aes.c aes.h
	gcc -c $(CFLAGS) aes.c -o $@

main.o: main.c aes.h
	gcc -c $(CFLAGS) main.c -o $@

aes: main.o aes.o
	gcc $(CLFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -rf *.o testes comparacao.png data.csv

purge: clean
	rm -f aes
