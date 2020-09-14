CC=gcc
CFLAGS=

PROGRAMS=bbcls bbccp bbcmk

DEPS=common.h
LIBRARY=bbc.o

all:	$(PROGRAMS)

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

bbcls:	bbcls.o $(LIBRARY) 
	$(CC) -o bbcls bbcls.o bbc.o

bbccp:	bbccp.o $(LIBRARY)
	$(CC) -o bbccp bbccp.o bbc.o

bbcmk:	bbcmk.o $(LIBRARY)
	$(CC) -o bbcmk bbcmk.o bbc.o

clean:
	rm -f *.o $(PROGRAMS)

