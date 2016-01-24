#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <sys/filio.h>
#include "control.h"

/*******************************************************
*                   Main    Function                   *
********************************************************/
main ()
{
    open_configure();
    printf("This relay has model name -> %s",get_model_name());
    printf("The version number is -> %s",get_version_number());
    printf("Relay status 1 is -> %d.\n", get_relay_status(1));
    printf("Relay status 2 is -> %d.\n", get_relay_status(2));
    printf("Relay status 3 is -> %d.\n", get_relay_status(3));
    printf("Relay status 4 is -> %d.\n", get_relay_status(4));
    printf("%s",switch_relay(1,1));
    printf("%s",switch_relay(2,1));
    printf("%s",switch_relay(3,1));
    printf("%s",switch_relay(4,1));
    printf("Relay status 1 is -> %d.\n", get_relay_status(1));
    printf("Relay status 2 is -> %d.\n", get_relay_status(2));
    printf("Relay status 3 is -> %d.\n", get_relay_status(3));
    printf("Relay status 4 is -> %d.\n", get_relay_status(4));
    printf("%s",switch_relay(1,0));
    printf("%s",switch_relay(2,0));
    printf("%s",switch_relay(3,0));
    printf("%s",switch_relay(4,0)); 
    printf("Relay status 1 is -> %d.\n", get_relay_status(1));
    printf("Relay status 2 is -> %d.\n", get_relay_status(2));
    printf("Relay status 3 is -> %d.\n", get_relay_status(3));
    printf("Relay status 4 is -> %d.\n", get_relay_status(4));
    close_device();
}

