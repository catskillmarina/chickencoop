controls.o:	controls.c control.h
CC = cc

all:		controls.o switches

switches:	
	$(CC) -o switches switches.c controls.o
	
clean:
	rm controls.o

distclean:
	rm controls.o switches
