#include <stdio.h>
#include <stdlib.h>
#include <termcap.h>
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
char response;
char dummy[255];

/*************************************************************
*                       clear_screen                         *
*************************************************************/
void clear_screen()
{
    char buf[1024];
    char *str;

    tgetent(buf, getenv("TERM"));
    str = tgetstr("cl", NULL);
    fputs(str, stdout);
} 


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


/*************************************************************
*                           main                             *
**************************************************************/
main ()
{
   open_configure();
   clear_screen();
   while(1)
   {
       printf("list relay status -> s\n");
       printf("turn relays on    -> O\n");
       printf("turn relays off   -> K\n");
       printf("turn a relay on   -> o\n");
       printf("turn a relay off  -> k\n");
       printf("get model name    -> m\n");
       printf("get version       -> v\n");
       printf("quit              -> q\n");
       printf("\n\n");
       printf("-------------> ");
       
       scanf("%c",&response);
       printf("response is -> %c\n",response);
       clear_screen();

       switch(response) {
          case 's' :
             printf("Relay status 1 is -> %d.\n", get_relay_status(1));
             printf("Relay status 2 is -> %d.\n", get_relay_status(2));
             printf("Relay status 3 is -> %d.\n", get_relay_status(3));
             printf("Relay status 4 is -> %d.\n", get_relay_status(4));
             scanf("%d\n",&dummy);
             break;
          case 'O' :
             printf("Relay 1 -> %s\n",switch_relay(1,1));
             printf("Relay 2 -> %s\n",switch_relay(2,1));
             printf("Relay 3 -> %s\n",switch_relay(3,1));
             printf("Relay 4 -> %s\n",switch_relay(4,1));
             scanf("%d\n",&dummy);
             break;
          case 'K' :
             printf("Relay 1 -> %s\n",switch_relay(1,0));
             printf("Relay 2 -> %s\n",switch_relay(2,0));
             printf("Relay 3 -> %s\n",switch_relay(3,0));
             printf("Relay 4 -> %s\n",switch_relay(4,0)); 
             scanf("%d\n",&dummy);
             break;
          case 'o' :
             printf("menu to turn single relay on");
             scanf("%d\n",&dummy);
             break;
          case 'k' :
             printf("menu to turn single relay off");
             scanf("%d\n",&dummy);
             break;
          case 'm' :
             printf("This relay has model name -> %s",get_model_name());
             scanf("%d\n",&dummy);
             break;
          case 'v' :
             printf("The version number is -> %s",get_version_number());
             scanf("%d\n",&dummy);
             break;
          case 'q' :
             exit(0);
          case 'b' :
          default :
             printf("Invalid grade\n" );
       }

    }
    close_device();
}
