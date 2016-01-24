OBJ = controls.o
CC = cc

switches:	$(OBJ)
	$(CC) -o switches switches.c control.h $(OBJ)
	
