#include <stdio.h>
#include <stdlib.h>




int main (int argc, char **argv)
{
    extern char *optarg;
    extern int optind;

    int sflag, oflag, kflag, mflag, vflag, hflag;

    int ch = 0;
     while ((ch = getopt(argc, argv, "s:o:k:mvh")) != -1) {
             switch (ch) {
             case 's':
                     sflag = 1;
                     printf("s optarg is %s\n", optarg);
                     break;
             case 'o':
                     oflag = 1;
                     printf("o optarg is %s\n", optarg);
                     break;
             case 'k':
                     oflag = 1;
                     printf("k optarg is %s\n", optarg);
                     break;
             case 'm':
                     printf("model name printed here\n");
                     break;
             case 'v':
                     oflag = 1;
                     printf("version printed here\n");
                     break;
             case '?': case 'h':
                     printf("This is how you use the command.\n");
                     printf("-s nnn # for statuses of relays\n");
                     printf("-o nnn # to turn on relays\n");
                     printf("-k nnn # to turn off relays\n");
                     printf("-m # to get model number\n");
                     printf("-v # to get version\n");
                     printf("-h or -?  # to get help\n");
                     break;
             default:
                     printf("Usage wrong - try -h \n");
             }
     }
     argc -= optind;
     argv += optind;
}



