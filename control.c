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
int n, speed, i, bytes_waiting = 0;
struct termios options;
char input_buffer[255];

/*******************************************************
*                  open_configure                      *
*******************************************************/
void open_configure()
{
    int fcarg;

    printf("In open_configure !\n");
    serial_filedesc = open("/dev/ttyU0", O_RDWR | O_NOCTTY | O_NDELAY);
    if(!serial_filedesc)
    {
        perror("could not open /dev/ttyU0 -");
    }
    else
    {
        printf("Serial port /dev/ttyU0 open filedesc %d \n.", n);
    }
    tcgetattr(serial_filedesc,&options);
    speed = cfsetispeed(&options,B115200);
    printf("speed set to %d.\n",speed);
    speed = cfsetospeed(&options,B115200);
    printf("speed set to %d.\n",speed);
    options.c_cflag |= (CLOCAL | CREAD );

    /* 8N1 */
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;

    /* from tip.c from netbsd */
    if ((fcarg = fcntl(serial_filedesc, F_GETFL, 0)) < 0 ||
             fcntl(serial_filedesc, F_SETFL, fcarg & ~O_NONBLOCK) < 0) {
                err(1, "can't clear O_NONBLOCK");
    } 
    /* fcntl(serial_filedesc, F_SETFL, FNDELAY);*/

    /* set raw input, 1 second timeout */
    options.c_cflag     |= (CLOCAL | CREAD);
    options.c_lflag     &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_oflag     &= ~OPOST;
    options.c_cc[VMIN]  = 0;
    options.c_cc[VTIME] = 10;

    /* set the options */
    tcsetattr(serial_filedesc, TCSANOW, &options);
    printf("Leaving open_configure !\n");
}


/******************************************************
*                     close_device                    *
******************************************************/
int close_device ()
{
    close(serial_filedesc);

    return(0);
}

/******************************************************
*                  re_open_device                     *
*******************************************************/
void re_open_device ()
{
    int j = 0;

    printf("in -- re_open_device -- Letting it rest for a few seconds i = %d !\n", i);
    {
        tcflow(serial_filedesc,TCOON);
        close_device();
        for (j=0;j<=12;j++)
            usleep(700000);
        open_configure();
    }
}

/******************************************************
*                  get_model_name                     *
*******************************************************/

get_model_name()
{
    int j;
    printf("In get Model Name !\n");
    bzero(input_buffer, 255);

    bytes_waiting = 0;
    n = 0;
    i = 0;
    while(bytes_waiting <= 0)
    {
        i++;
        n = write(serial_filedesc,"GI\n",3);
        tcdrain(serial_filedesc);
        ioctl(serial_filedesc, FIONREAD, &bytes_waiting);
        n = read(serial_filedesc,input_buffer,bytes_waiting); 
        printf("GMN There are %d bytes waiting to be read. -> %s\n", bytes_waiting, input_buffer);
        usleep(500000);

        if(i >= 20)
        {
            re_open_device();
            i = 0;
        }
             
    }
    printf("This is how many chars written %d.\n", n);
    n = read(serial_filedesc,input_buffer,bytes_waiting); 
    printf("%s\n",input_buffer);
    printf("Leaving get Model Name !\n");
}

/******************************************************
*                  get_version_number                 *
*******************************************************/
get_version_number()
{
    int i =0;
    int j = 0;

    printf("In Get version Number !\n");
    bzero(input_buffer, 255);
    bytes_waiting = 0;
    n = 0;
    while(bytes_waiting <= 0)
    {
        n = write(serial_filedesc,"GV\n",3);
        tcdrain(serial_filedesc);
        ioctl(serial_filedesc, FIONREAD, &bytes_waiting);
        n = read(serial_filedesc,input_buffer,bytes_waiting); 
        printf("GET VER There are %d bytes waiting to be read. -> %s \n", bytes_waiting, input_buffer);
        usleep(500000);
        if(i >= 20)
        {
            re_open_device(); 
            i = 0;
        }
    }
    printf("This is how many chars written %d. \n", n);
    n = read(serial_filedesc,input_buffer,bytes_waiting); 
    printf("%s\n",input_buffer);
    printf("In Get version Number !\n");
}

/******************************************************
*                  get_relay_status                   *
*******************************************************/
int get_relay_status(int relay)
{
    int i = 0, j=0;

    printf("In get_relay_status %d!\n", relay);
    bzero(input_buffer, 255);
    bytes_waiting = 0;
    n = 0;
    char foo[256];
    bzero(foo,255);
    sprintf(foo, "G%d\n", relay);
    while(bytes_waiting <= 0)
    {
        i++;
        n = write(serial_filedesc,foo,3);
        ioctl(serial_filedesc, FIONREAD, &bytes_waiting);
        n = read(serial_filedesc,input_buffer,bytes_waiting); 
        tcdrain(serial_filedesc);
        usleep(500000); 
        printf("GET RELAY STATUS There are %d bytes waiting to be read. -> %s \n", bytes_waiting, input_buffer);
        if(i >= 20)
        {
            re_open_device();
            i = 0;
        }

    }
    printf("This is how many chars written %d. \n", n);
    n = read(serial_filedesc,input_buffer,bytes_waiting); 
    tcdrain(serial_filedesc);
    printf("Relay Status ---> %s \n", input_buffer);
    printf("Leaving get_relay_status !\n");

    return atoi(input_buffer);
}

/******************************************************
*                  switch_relay                       *
*******************************************************/
int switch_relay(int relay,int action)
{
    int bytes_waiting = 0;
    int n = 0;
    int i = 0;
    int j = 0;
    char foo[256];

    printf("/*************************************************/\n");
    printf("In switch_relayRelay -> %d! Action -> \n", relay, action);
    bzero(input_buffer, 255);
    bzero(foo,255);
    sprintf(foo, "S%d=%d\n", relay,action);
    printf("%s\n",foo);

    while(bytes_waiting <= 0)
    {
        i++;
        n = write(serial_filedesc,foo,5);
        tcdrain(serial_filedesc);
        ioctl(serial_filedesc, FIONREAD, &bytes_waiting);
        n = read(serial_filedesc,input_buffer,bytes_waiting); 
        usleep(500000);
        printf("switch_relay There are %d bytes waiting 2 be read. -> %s \n", bytes_waiting, input_buffer);
        if(i >= 20)
        {
            re_open_device();
            i = 0;
        }

    }
    printf("This is how many chars written %d. \n", n);
    n = read(serial_filedesc,input_buffer,bytes_waiting); 
    printf("switch_relay ---> %s \n", input_buffer);
    printf("Leaving switch_relay !\n");
    printf("/*************************************************/\n");
}

/*******************************************************
*                   Main    Function                   *
********************************************************/
main ()
{

    open_configure();
/*    get_model_name();
    usleep(500000);usleep(500000);usleep(500000);
    get_version_number();
*/

    printf("Relay status 1 is -> %d.\n", get_relay_status(1));
    printf("Relay status 2 is -> %d.\n", get_relay_status(2));
    printf("Relay status 3 is -> %d.\n", get_relay_status(3));
    printf("Relay status 4 is -> %d.\n", get_relay_status(4));

/*
    usleep(500000);usleep(500000);usleep(500000); 
    switch_relay(1,1);
    usleep(500000);usleep(500000);usleep(500000);
    switch_relay(2,1);
    usleep(500000);usleep(500000);usleep(500000);
    switch_relay(3,1);
    usleep(500000);usleep(500000);usleep(500000);
    switch_relay(4,1);
    usleep(500000);usleep(500000);usleep(500000);
    switch_relay(1,0);
    usleep(500000);usleep(500000);usleep(500000);
    switch_relay(2,0);
    usleep(500000);usleep(500000);usleep(500000);
    switch_relay(3,0);
    usleep(500000);usleep(500000);usleep(500000);
    switch_relay(4,0);
    usleep(500000);usleep(500000);usleep(500000);
*/

    /* fcntl(serial_filedesc, F_SETFL, 0); */
    
}

