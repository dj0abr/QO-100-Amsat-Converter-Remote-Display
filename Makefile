CFLAGS?=-O3 -Wall -std=gnu99
LDLIBS+= -lpthread -lm
CC?=gcc
PROGNAME=amsatdisplay
OBJ=amsatdisplay.o serial.o pipe.o evaldata.o

all: amsatdisplay

amsatdisplay: $(OBJ)
	$(CC) -g -o $@ $^ $(LDFLAGS) $(LDLIBS)

clean:
	rm -f *.o amsatdisplay

install:
	chmod 755 amsatdisplay ad
	sudo ./amsatdisplay
	
