/*
 * ex: softtabstop=4 shiftwidth=4 tabstop=4 expandtab
 *
 * Copyright (c) 2017 Next Thing Co
 * All rights reserved
 */

#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include "mongoose.h"

#include "config.h"
#include "utils.h"
#include "gadget_project.h"
#include "mongoose_utils.h"

static int verbose;

void gadget_start_help()
{
    printf(
            "Create embedded Linux apps - easy.\n"
            "\n"
            "usage: gadget start [<project_path>]\n"
            "\n"
            "optional arguments:\n"
            "  -h, --help            show this help message and exit\n"
            "  --verbose             be verbose\n"
            "  <project_path>        start project in path (default: .)\n"
          );
}

int gadget_start(int argc,char **argv)
{
    int c, ret=0;
    char   *project_path=NULL;
    gadget_project_t *project=0;

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
                gadget_start_help();
                ret=0;
                goto _return;

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
        fprintf(stderr,"gadget start: ERROR, unknown extra arguments: ");
        while (optind < argc)
            fprintf (stderr,"%s ", argv[optind++]);
        putchar ('\n');
        ret = -1;
        goto _return;
    }

    if(!xis_dir("%s/.gadget",project_path)) {
        fprintf(stderr,"gadget start: ERROR: not a gadget project: '%s'\n",project_path);
        ret=1;
        goto _return;
    }

    if(!(project=gadget_project_deserialize("%s/.gadget/config",project_path))) {
        fprintf(stderr,"gadget start: ERROR: cannot read project file: '%s/.gadget/config'\n",project_path);
        goto _return;
    }
    
    do_rpc(ENDPOINT_APPLICATION_START,project);
    
_return:
    if(project) gadget_project_destruct(project);

    return 0;
}
