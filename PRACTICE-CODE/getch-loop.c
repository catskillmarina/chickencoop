#include <sys/time.h>
#include <sys/types.h>
#include <sys/filio.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

void main(void)
{
    int filein = 0; 
    int maxfd;
    fd_set readfs;
    int bytes_waiting;
    char input_buffer[255];
    int n;

    while(1)
    {
        FD_SET(filein, &readfs);
        select(maxfd,&readfs,NULL,NULL,NULL);
        if(FD_ISSET(filein, &readfs))
        {
            ioctl(filein, FIONREAD, &bytes_waiting);
            bzero(input_buffer,254);
            n = read(filein,input_buffer,bytes_waiting);
            write(1,input_buffer,n);
        }
    }
}
