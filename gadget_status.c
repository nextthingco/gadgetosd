/*
 * ex: softtabstop=4 shiftwidth=4 tabstop=4 expandtab
 *
 * Copyright (c) 2017 Next Thing Co
 * All rights reserved
 */

#include <stdio.h>
#include <unistd.h>
#include <getopt.h>

#include "config.h"
#include "utils.h"
#include "gadget_project.h"
#include "mongoose_utils.h"

void gadget_status_help()
{
    xprint( NORMAL,
            "Create embedded Linux apps - easy.\n"
            "\n"
            "usage: gadget status\n"
            "\n"
            "optional arguments:\n"
            "  -h, --help            show this help message and exit\n"
            "  -v, --verbose         be verbose\n"
          );
}

int gadget_status(int argc,char **argv)
{
    int c, ret=0;

    while (1)
    {
        static struct option long_options[] =
        {
            {"verbose", no_argument,       0, 'v'},
            {"help",    no_argument,       0, 'h'},
            {0, 0, 0, 0}
        };

        int option_index = 0;

        c = getopt_long (argc, argv, "hv",
                long_options, &option_index);

        /* Detect the end of the options. */
        if (c == -1)
            break;

        switch (c)
        {
            case 'v':
                _VERBOSE=1;
                break;

            case 'h':
                gadget_status_help();
                ret=0;
                goto _return;

            case '?':
                /* getopt_long already printed an error message. */
                break;

            default:
                abort ();
        }
    }

    if(optind < argc) {
        xprint(ERROR,"gadget build: ERROR, unknown extra arguments: ");
        while (optind < argc)
            xprint(ERROR, "%s ", argv[optind++]);
        putchar ('\n');
        ret = -1;
        goto _return;
    }

    do_rpc(ENDPOINT_APPLICATION_STATUS,0);

_return:
    return ret;
}
