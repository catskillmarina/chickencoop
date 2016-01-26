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
                        printf("Relay status ZZ -> %c.\n", get_relay_status(tempint));
                    }
                    break;
            case 'S':
                    printf("Relay status 1 is -> %d.\n", get_relay_status(1));
                    printf("Relay status 2 is -> %d.\n", get_relay_status(2));
                    printf("Relay status 3 is -> %d.\n", get_relay_status(3));
                    printf("Relay status 4 is -> %d.\n", get_relay_status(4));
                    break;
            case 'o':
                    oflag = 1;
                    for(i=0;optarg[i]!='\0';i++)
                    {
                        tempstr[0]=optarg[i];
                        tempstr[1] = '\0';
                        tempint = atoi(tempstr);
                        printf("Relay -> %s\n",switch_relay(tempint,1));
                    }
                    break;
            case 'O':
                    oflag = 1;
                    printf("Relay 1 -> %s\n",switch_relay(1,1));
                    printf("Relay 2 -> %s\n",switch_relay(2,1));
                    printf("Relay 3 -> %s\n",switch_relay(3,1));
                    printf("Relay 4 -> %s\n",switch_relay(4,1));
                    break;
            case 'k':
                    kflag = 1;
                    for(i=0;optarg[i]!='\0';i++)
                    {
                        tempstr[0]=optarg[i];
                        tempstr[1] = '\0';
                        tempint = atoi(tempstr);
                        printf("Relay -> %s\n",switch_relay(tempint,0));
                    }
                    break;
            case 'm':
                    printf("%s",get_model_name());
                    break;
            case 'v':
                    vflag = 1;
                    printf("%s",get_version_number());
                    break;
            case '?': case 'h':
                    printf("This is how you use the command.\n");
                    printf("coopcontrol -s nnn         # for statuses of relays\n");
                    printf("coopcontrol -S             # for statuses of ALL relays\n");
                    printf("coopcontrol -o nnn         # to turn on relays\n");
                    printf("coopcontrol -O             # to turn on ALL relays\n");
                    printf("coopcontrol -k nnn         # to turn off relays\n");
                    printf("coopcontrol -m             # to get model number\n");
                    printf("coopcontrol -v             # to get version\n");
                    printf("coopcontrol -h or -?       # to get help\n");
                    break;
            default:
                    printf("Usage wrong - try -h \n");
            }
    }
    close_device();
}



