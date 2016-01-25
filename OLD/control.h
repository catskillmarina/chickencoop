#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <sys/filio.h>

/******************************************************
*                  Global Variables                   *
*******************************************************/
extern int serial_filedesc;
extern int n; 
extern int speed; 
extern int i; 
extern int bytes_waiting;
extern struct termios options;
extern char input_buffer[255];


