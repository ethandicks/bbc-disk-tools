CC=gcc

OBJECTS=bbc.o bbcls.o bbccp.o bbcmk.o

*.o:	$(OBJECTS)

bbcls:	$(OBJECTS)
	$(CC) -o bbcls bbcls.o bbc.o

bbccp:	$(OBJECTS)
	$(CC) -o bbccp bbccp.o bbc.o

bbcmk:	$(OBJECTS)
	$(CC) -o bbcmk bbcmk.o bbc.o

all:	$(OBJECTS) bbcls bbccp bbcmk

