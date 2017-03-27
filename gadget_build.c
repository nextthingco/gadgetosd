/*
 * ex: softtabstop=4 shiftwidth=4 tabstop=4 expandtab
 *
 * Copyright (c) 2017 Next Thing Co
 * All rights reserved
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>
#include <limits.h>
#include "utils.h"
#include "gadget_project.h"

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
    int c,ret=0;
    char *project_path=0,*project_filename=".gadget/config",*cmd=0,*container_name=0;
    char pwd[PATH_MAX]; //previous working dir
    char *export_cmd=NULL;
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
                gadget_build_help();
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
        fprintf(stderr,"gadget build: ERROR, unknown extra arguments: ");
        while (optind < argc)
            fprintf (stderr,"%s ", argv[optind++]);
        putchar ('\n');
        ret = -1;
        goto _return;
    }


    if(!getcwd(pwd,PATH_MAX)) {
        fprintf(stderr,"gadget build: ERROR: cannot find out current directory");
        ret=errno;
    }

    if(chdir(project_path)) {
        fprintf(stderr,"gadget build: ERROR: cannot access project path '%s'\n",project_path);
        ret=errno;
        goto _return;
    }

    if(!xis_dir(".gadget")) {
        fprintf(stderr,"gadget build: ERROR: not a gadget project: '%s'\n",project_path);
        ret=1;
        goto _return;
    }

    if(!(project=gadget_project_deserialize(project_filename)))
    {
        fprintf(stderr,"gadget build: ERROR: cannot read gadget project file: '%s'\n",project_filename);
        ret=1;
        goto _return;
    }

    asprintf(&container_name,"%s_%s",project->name,project->id);

    asprintf(&cmd,"docker build -t %s .",container_name);
    if(system(cmd)) {

        fprintf(stderr,"gadget build: ERROR: calling '%s' failed\n",cmd);
        ret=errno;
        goto _return;
    }
    free(cmd);

    asprintf(&cmd,"docker save %s -o %s.tar",container_name,container_name);
    if(system(cmd)) {

        fprintf(stderr,"gadget build: ERROR: calling '%s' failed\n",cmd);
        ret=errno;
        goto _return;
    }

_return:
    if(export_cmd) free(export_cmd);
    if(cmd) free(cmd);
    if(container_name) free(container_name);

    if(chdir(pwd)) {
        fprintf(stderr,"gadget build: ERROR: cannot return to previous directory '%s'\n",pwd);
        ret=errno;
    }

    return ret;
}

