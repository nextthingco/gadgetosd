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
#include "config.h"
#include "gadget_project.h"


void gadget_build_help()
{
    printf(
            "Create embedded Linux apps - easy.\n"
            "\n"
            "usage: gadget build [<project_path>]\n"
            "\n"
            "optional arguments:\n"
            "  -h, --help            show this help message and exit\n"
            "  -v, --verbose         be verbose\n"
            "  <project_path>        build project in path (default: .)\n"
          );
}


int gadget_build(int argc, char **argv)
{
    int c,ret=0;
    char *project_path=0,*project_filename=".gadget/config",*container_name=0,*container_filename=0;
    char pwd[PATH_MAX]; //previous working dir
    gadget_project_t *project=0;
    subprocess_t *p=0;

    while (1)
    {
        static struct option long_options[] =
        {
            {"help",    no_argument,       0, 'h'},
            {"verbose", no_argument,       0, 'v'},
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

    if(optind == argc) {
        project_path=".";
    } else {
        project_path=argv[optind++];
    }

    if(optind < argc) {
        xprint(ERROR,"gadget build: ERROR, unknown extra arguments: ");
        while (optind < argc)
            xprint(ERROR,"%s ", argv[optind++]);
        putchar ('\n');
        ret = -1;
        goto _return;
    }


    if(!getcwd(pwd,PATH_MAX)) {
        xprint(ERROR,"gadget build: ERROR: cannot find out current directory");
        ret=errno;
    }

    if(chdir(project_path)) {
        xprint(ERROR,"gadget build: ERROR: cannot access project path '%s'\n",project_path);
        ret=errno;
        goto _return;
    }

    if(!xis_dir(".gadget")) {
        xprint(ERROR,"gadget build: ERROR: not a gadget project: '%s'\n",project_path);
        ret=1;
        goto _return;
    }

    if(!(project=gadget_project_deserialize(project_filename)))
    {
        xprint(ERROR,"gadget build: ERROR: cannot read gadget project file: '%s'\n",project_filename);
        ret=1;
        goto _return;
    }

    asprintf(&container_name,"%s_%s",project->name,project->id);

    p=subprocess_run_gw("docker","build","-t",container_name,".",0);
    if(p->exit) {
        xprint(ERROR,"gadget build: ERROR: start subprocess '%s' failed:\n\n",p->cmdline);
        xprint(ERROR,"%s\n%s%s\n", p->cmdline, p->out, p->err);
        goto _return;
    }
    xprint(VERBOSE,"%s\n%s%s\n", p->cmdline, p->out, p->err);
    subprocess_free(p); p=0;

    asprintf(&container_filename,"%s.tar",container_name);
    p=subprocess_run_gw("docker","save",container_name,"-o",container_filename,0);
    if(p->exit) {
        xprint(ERROR,"gadget build: ERROR: start subprocess '%s' failed:\n\n",p->cmdline);
        xprint(ERROR,"%s\n%s%s\n", p->cmdline, p->out, p->err);
        goto _return;
    }
    xprint(VERBOSE,"%s\n%s%s\n", p->cmdline, p->out, p->err);
    subprocess_free(p); p=0;

_return:
    if(p)                  subprocess_free(p);
    if(container_name)     free(container_name);
    if(container_filename) free(container_filename);

    if(chdir(pwd)) {
        xprint(ERROR,"gadget build: ERROR: cannot return to previous directory '%s'\n",pwd);
        ret=errno;
    }

    return ret;
}

