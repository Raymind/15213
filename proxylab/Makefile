CC = gcc
CFLAGS = -g -Wall -Werror
LDFLAGS = -lpthread

all: proxy

csapp.o: csapp.c csapp.h
	$(CC) $(CFLAGS) -c csapp.c

proxy.o: proxy.c proxy.h
	$(CC) $(CFLAGS) -c proxy.c

proxy: proxy.o csapp.o cache.o

cache.o: cache.c cache.h
	$(CC) $(CFLAGS) -c cache.c

submit:
	(make clean; cd ..; tar cvf proxylab.tar proxylab)

clean:
	rm -f *~ *.o proxy core

