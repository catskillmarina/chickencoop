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
int serial_filedesc;
int n; 
int speed; 
int i; 
int bytes_waiting = 0;
struct termios options;
char input_buffer[255];


