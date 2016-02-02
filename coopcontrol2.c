#include <stdio.h>
#include <stdlib.h>
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
int bytes_waiting;
struct termios options;
char input_buffer[255];
int failcounter;


/*******************************************************
*                  open_configure                      *
*******************************************************/
void open_configure()
{
    int fcarg;

    serial_filedesc = open("/dev/ttyU0", O_RDWR | O_NOCTTY | O_NDELAY);
    if(!serial_filedesc)
    {
        perror("could not open /dev/ttyU0 -");
    }
    tcgetattr(serial_filedesc,&options);
    speed = cfsetispeed(&options,B115200);
    speed = cfsetospeed(&options,B115200);
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
*                  let_relay_rest                     *
******************************************************/
void let_relay_rest(void)
{
    int mcs = 100000;

    usleep(mcs);
    usleep(mcs); 
    usleep(mcs);
}

/******************************************************
*                  re_open_device                     *
*******************************************************/
void re_open_device ()
{
    int j = 0;

    {
        tcflow(serial_filedesc,TCOON);
        close_device();
        usleep(400000);
        open_configure();
    }
}

/******************************************************
*                  get_model_name                     *
*******************************************************/

char *get_model_name()
{
    int j, i = 0, n = 0;

    bzero(input_buffer, 255);
    bytes_waiting = -1;
    while(bytes_waiting <= 0)
    {
        i++;
        n = write(serial_filedesc,"GI\n",3);
        tcdrain(serial_filedesc);
        ioctl(serial_filedesc, FIONREAD, &bytes_waiting);
        n = read(serial_filedesc,input_buffer,bytes_waiting); 
        if(i >= 5)
        {
            re_open_device();
            i = 0;
        }
    }
    return input_buffer;
}

/******************************************************
*                  get_version_number                 *
*******************************************************/
char *get_version_number()
{
    int i =0;
    int j = 0;

    bzero(input_buffer, 255);
    n = 0;
    bytes_waiting = -1;
    while(bytes_waiting <= 0)
    {
        n = write(serial_filedesc,"GV\n",3);
        tcdrain(serial_filedesc);
        ioctl(serial_filedesc, FIONREAD, &bytes_waiting);
        n = read(serial_filedesc,input_buffer,bytes_waiting); 
        if(i >= 5)
        {
            re_open_device(); 
            i = 0;
        }
        i++;
    }
    return input_buffer;
}

/******************************************************
*                  get_relay_status                   *
*******************************************************/
int get_relay_status(int relay)
{
    int i = 0, j=0;

    bzero(input_buffer, 255);
    n = 0;
    char foo[256];
    bzero(foo,255);
    sprintf(foo, "G%d\n", relay);
    bytes_waiting = -1;
    while(bytes_waiting <= 0)
    {
        i++;
        n = write(serial_filedesc,foo,3);
        tcdrain(serial_filedesc);
        ioctl(serial_filedesc, FIONREAD, &bytes_waiting);
        n = read(serial_filedesc,input_buffer,bytes_waiting); 
        if(i >= 5)
        {
            re_open_device();
            i = 0;
        }
    }
    return atoi(input_buffer);
}

/******************************************************
*                  switch_relay                       *
*******************************************************/
char *switch_relay(int relay,int action)
{
    int n = 0;
    int i = 0;
    int j = 0;
    char foo[256];

    bzero(input_buffer, 255);
    bzero(foo,255);
    sprintf(foo, "S%d=%d\n", relay,action);

    bytes_waiting = -1;
    while(bytes_waiting <= 0)
    {
        i++;
        n = write(serial_filedesc,foo,5);
        tcdrain(serial_filedesc);
        ioctl(serial_filedesc, FIONREAD, &bytes_waiting);
        n = read(serial_filedesc,input_buffer,bytes_waiting); 
        if(i >= 5)
        {
            re_open_device();
            i = 0;
        }
    }
    return input_buffer;
}

/******************************************************
*                   main function                     *
*******************************************************/
int main (int argc, char **argv)
{
    extern char *optarg;
    extern int optind;

    int sflag, oflag, kflag, mflag, vflag, hflag;
    int i;
    char tempstr[25];
    int tempint;
    char outstr[80];

    int ch = 0;
    open_configure();
    while ((ch = getopt(argc, argv, "Ss:o:k:mvh")) != -1) {
            switch (ch) {
            case 's':
                    sflag = 1;
                    for(i=0;optarg[i]!='\0';i++)
                    {
                        tempstr[0]=optarg[i];
                        tempstr[1] = '\0';
                        tempint = atoi(tempstr);
                        bzero(outstr,80);
                        sprintf(outstr,"Relay status %d -> %c tempint -> %d.\n", i,get_relay_status(tempint),tempint);
                        write(1,outstr,80);
                    }
                    break;
            case 'S':
                    bzero(outstr,80);
                    sprintf(outstr,"Relay status 1 is -> %d.\n\r", get_relay_status(1));
                    write(1,outstr,80);

                    bzero(outstr,80);
                    sprintf(outstr,"Relay status 2 is -> %d.\n\r", get_relay_status(2));
                    write(1,outstr,80);

                    bzero(outstr,80);
                    sprintf(outstr,"Relay status 3 is -> %d.\n\r", get_relay_status(3));
                    write(1,outstr,80);

                    bzero(outstr,80);
                    sprintf(outstr,"Relay status 4 is -> %d.\n\r", get_relay_status(4));
                    write(1,outstr,80);

                    break;
            case 'o':
                    oflag = 1;
                    for(i=0;optarg[i]!='\0';i++)
                    {
                        tempstr[0]=optarg[i];
                        tempstr[1] = '\0';
                        tempint = atoi(tempstr);
                        bzero(outstr,80);
                        sprintf(outstr,"Relay -> %s\n\r",switch_relay(tempint,1));
                        write(1,outstr,80);
                    }
                    break;
            case 'O':
                    bzero(outstr,80);
                    sprintf(outstr,"Relay 1 -> %s\n\r",switch_relay(1,1));
                    write(1,outstr,80);

                    bzero(outstr,80);
                    sprintf(outstr,"Relay 2 -> %s\n\r",switch_relay(2,1));
                    write(1,outstr,80);

                    bzero(outstr,80);
                    sprintf(outstr,"Relay 3 -> %s\n\r",switch_relay(3,1));
                    write(1,outstr,80);

                    bzero(outstr,80);
                    sprintf(outstr,"Relay 4 -> %s\n\r",switch_relay(4,1));
                    write(1,outstr,80);

                    break;
            case 'k':
                    for(i=0;optarg[i]!='\0';i++)
                    {
                        tempstr[0]=optarg[i];
                        tempstr[1] = '\0';
                        tempint = atoi(tempstr);
                        bzero(outstr,80);
                        sprintf(outstr,"Relay -> %s\n\r",switch_relay(tempint,0));
                        write(1,outstr,80);
                    }
                    break;
            case 'm':
                    bzero(outstr,80);
                    sprintf(outstr,"%s",get_model_name());
                    write(1,outstr,80);
                    break;
            case 'v':
                    bzero(outstr,80);
                    sprintf(outstr,"%s",get_version_number());
                    write(1,outstr,80);
                    break;
            case '?': case 'h':
                    write(1,"This is how you use the command.                       \n\r",56);
                    write(1,"coopcontrol -s nnn         # for statuses of relays    \n\r",56);
                    write(1,"coopcontrol -S             # for statuses of ALL relays\n\r",56);
                    write(1,"coopcontrol -o nnn         # to turn on relays         \n\r",56);
                    write(1,"coopcontrol -O             # to turn on ALL relays     \n\r",56);
                    write(1,"coopcontrol -k nnn         # to turn off relays        \n\r",56);
                    write(1,"coopcontrol -m             # to get model number       \n\r",56);
                    write(1,"coopcontrol -v             # to get version            \n\r",56);
                    write(1,"coopcontrol -h or -?       # to get help               \n\r",56);
                    break;
            default:
                    printf("Usage wrong - try -h \n");
            }
    }
    close_device();
}



