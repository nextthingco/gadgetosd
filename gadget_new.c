#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <sys/stat.h>
#include "utils.h"

static int verbose=0;
static char* type="docker";

void help()
{
    printf(
            "Create embedded Linux apps - easy.\n"
            "\n"
            "usage: gadget new [<args>] <name>\n"
            "\n"
            "optional arguments:\n"
            "  -h, --help            show this help message and exit\n"
            "  -t TYPE, --type=TYPE  specify project type (default: docker)\n"
            "\n"
            "project types:\n"
            "  docker                Docker based project\n"
          );
}

int gadget_new(int argc, char **argv)
{
    int c;
    char *target_dir=0,*target_file=0;

    while (1)
    {
        static struct option long_options[] =
        {
            {"verbose", no_argument,       &verbose, 1},
            {"help",    no_argument,       0, 'h'},
            {"type",    required_argument, 0, 't'},
            {0, 0, 0, 0}
        };

        int option_index = 0;

        c = getopt_long (argc, argv, "ht:",
                long_options, &option_index);

        /* Detect the end of the options. */
        if (c == -1)
            break;

        switch (c)
        {
            case 0:
                /* If this option set a flag, do nothing else now. */
                if (long_options[option_index].flag != 0)
                    break;
                printf ("option %s", long_options[option_index].name);
                if (optarg)
                    printf (" with arg %s", optarg);
                printf ("\n");
                break;

            case 'h':
                help();
                return 0;

            case 't':
                type=optarg;
                break;

            case '?':
                /* getopt_long already printed an error message. */
                break;

            default:
                abort ();
        }
    }

    if (verbose)
        puts ("verbose flag is set\n");

    if(optind == argc) {
        printf("gadget new: ERROR: no project name given\n");
        return 1;
    }
    target_dir=argv[optind++];

    if(optind < argc) {
        optind++;
        printf("gadget new: ERROR, unknown extra arguments: ");
        while (optind < argc)
            printf ("%s ", argv[optind++]);
        putchar ('\n');
        return 1;
    }

    if(strcmp(type,"docker")) {
        printf("gadget new: ERROR, unknown project types: '%s'\n",type);
        return 1;
    }

    if(mkdir(target_dir,0775)) {
        printf("gadget new: ERROR: cannot create target directory '%s'\n",target_dir);
        return 1;
    }

    cp("/usr/share/gadget/templates/alpine/Dockerfile",target_dir);
    cp("/usr/share/gadget/templates/alpine/blink-leds",target_dir);
    cp("/usr/share/gadget/templates/alpine/rootfs.tar.gz",target_dir);

    return 0;

}

