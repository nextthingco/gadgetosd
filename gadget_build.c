#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "utils.h"

static int verbose;

void gadget_build_help()
{
    printf(
            "Create embedded Linux apps - easy.\n"
            "\n"
            "usage: gadget build [<project_path>]\n"
            "\n"
            "optional arguments:\n"
            "  -h, --help            show this help message and exit\n"
            "  --verbose             be verbose\n"
            "  <project_path>        build project in path (default: .)\n"
          );
}


int gadget_build(int argc, char **argv)
{
    int c;
    char *project_path=0;
    
    while (1)
    {
        static struct option long_options[] =
        {
            {"verbose", no_argument,       &verbose, 1},
            {"help",    no_argument,       0, 'h'},
            {0, 0, 0, 0}
        };

        int option_index = 0;

        c = getopt_long (argc, argv, "h",
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
                printf("option %s", long_options[option_index].name);
                if (optarg)
                    printf (" with arg %s", optarg);
                printf ("\n");
                break;

            case 'h':
                gadget_build_help();
                return 0;

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
        project_path=".";
    } else {
        project_path=argv[optind++];
    }

    if(optind < argc) {
        fprintf(stderr,"gadget build: ERROR, unknown extra arguments: ");
        while (optind < argc)
            fprintf (stderr,"%s ", argv[optind++]);
        putchar ('\n');
        return 1;
    }

    fprintf(stderr,"project_path=%s\n",project_path);
}

