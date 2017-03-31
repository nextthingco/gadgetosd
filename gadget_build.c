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
    char *project_path=0,*project_filename=".gadget/config",*cmd=0,*cmd_output=0,*container_name=0;
    char pwd[PATH_MAX]; //previous working dir
    char *export_cmd=NULL;
    gadget_project_t *project=0;

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

    // that's how it should be (to be implemented soon):
    //p=xrun_w("docker","build","-t",container_name,".",0);
    //if(p.exit) {
    //    xprint(ERROR,"gadget build: ERROR: start subprocess '%s' failed: %s\n%s\n", p.cmd_line, p.out, p.err);
    //}

    int pipes[3]; int status; int pid;
    pid=xpopen(pipes,"docker", "build", "-t",container_name,".",0);
    if(pid<0) {
        xprint(ERROR,"gadget build: ERROR: start subprocess '%s' failed.\n");
    }
    waitpid(pid,&status,0);
    if(WIFEXITED(status)) {
        status = WEXITSTATUS(status);
        if(status) {
            xprint(ERROR,"gadget build: ERROR: calling 'docker' failed:\n");
            FILE *f; int k;
            f = fdopen(pipes[1],"r"); while((k=fgetc(f))!=EOF) putchar(k); fclose(f);
            f = fdopen(pipes[2],"r"); while((k=fgetc(f))!=EOF) putchar(k); fclose(f);
        }
    } else {
        xprint(VERBOSE,"ERROR: child quit abnormally\n");
    }
    if(pid<0 || status) goto _return;

    asprintf(&cmd,"docker save %s -o %s.tar",container_name,container_name);
    if(xrun(cmd,&cmd_output)) {
        xprint(ERROR,"gadget build: ERROR: calling '%s' failed:\n%s",cmd,cmd_output);
        ret=errno;
        goto _return;
    }
    xprint(VERBOSE,cmd_output);
    if(cmd_output) { free(cmd_output); cmd_output=0; }
    if(cmd) { free(cmd); cmd=0; }

_return:
    if(export_cmd) free(export_cmd);
    if(cmd) free(cmd);
    if(cmd_output) free(cmd_output);
    if(container_name) free(container_name);

    if(chdir(pwd)) {
        xprint(ERROR,"gadget build: ERROR: cannot return to previous directory '%s'\n",pwd);
        ret=errno;
    }

    return ret;
}

