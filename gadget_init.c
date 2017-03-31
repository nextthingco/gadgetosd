/*
 * ex: softtabstop=4 shiftwidth=4 tabstop=4 expandtab
 *
 * Copyright (c) 2017 Next Thing Co
 * All rights reserved
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <libgen.h>
#include <sys/stat.h>

#include "utils.h"
#include "config.h"
#include "gadget_project.h"

static char* type="docker";

void gadget_init_help()
{
    printf(
            "Create embedded Linux apps - easy.\n"
            "\n"
            "usage: gadget init [<args>] <name>\n"
            "\n"
            "optional arguments:\n"
            "  -h, --help            show this help message and exit\n"
            "  -v, --verbose         be verbose\n"
            "  -t TYPE, --type=TYPE  specify project type (default: docker)\n"
            "\n"
            "project types:\n"
            "  docker                Docker based project\n"
          );
}

int gadget_init(int argc, char **argv)
{
    int c=0, ret=0, cleanup=0;
    char *target_dir=0,*project_name=0,*tmpstr=0,*cmd=0;
    gadget_project_t* project=0;

    while (1)
    {
        static struct option long_options[] =
        {
            {"verbose", no_argument,       0, 'v'},
            {"help",    no_argument,       0, 'h'},
            {"type",    required_argument, 0, 't'},
            {0, 0, 0, 0}
        };

        int option_index = 0;

        c = getopt_long (argc, argv, "ht:v",
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
                gadget_init_help();
                ret=0;
                goto _return;

            case 't':
                type=optarg;
                break;

            case '?':
                goto _return;
                break;

            default:
                abort ();
        }
    }

    if(optind == argc) {
        xprint(ERROR,"gadget init: ERROR: no project name given\n");
        ret=1;
        goto _return;
    }
    target_dir=argv[optind++];

    if(optind < argc) {
        xprint(ERROR,"gadget init: ERROR, unknown extra arguments: ");
        while (optind < argc)
            printf ("%s ", argv[optind++]);
        putchar ('\n');
        ret=1;
        goto _return;
    }

    if(strcmp(type,"docker")) {
        xprint(ERROR,"gadget init: ERROR, unknown project types: '%s'\n",type);
        ret=1;
        goto _return;
    }

    if(mkdir(target_dir,0775)) {
        xprint(ERROR,"gadget init: ERROR: cannot create target directory '%s'\n",target_dir);
        ret=1;
        goto _return;
    }

    //at this point we have created a project directory,
    //which we need to cleanup if anything goes wrong
    cleanup=1;

    if( xmkdir(0775,"%s/%s",target_dir,".gadget") ) {
        xprint(ERROR,"gadget init: ERROR: cannot create directory '%s/%s'\n",target_dir,".gadget");
        ret=1;
        goto _return;
    }

    // doesn't copy executable flags... 
    //xcp("/usr/local/share/gadget/templates/alpine/dockerfile",target_dir);
    //xcp("/usr/local/share/gadget/templates/alpine/blink-leds",target_dir);
    //xcp("/usr/local/share/gadget/templates/alpine/rootfs.tar.gz",target_dir);

    if( access( TEMPLATE_PREFIX, F_OK) == -1 ) {
        xprint(ERROR, "gadget init: template prefix '%s' doesn't exist.\n", TEMPLATE_PREFIX);
        ret=1;
        goto _return;
    }
    // q'n'd fix:
    asprintf(&cmd,"cp -va %s/alpine/* %s/",TEMPLATE_PREFIX,target_dir);
    FILE* proc = popen(cmd, "r");
    int status = pclose(proc);
    if(status < 0) {
        xprint(ERROR, "gadget init: ERROR copying template into new '%s'\n", target_dir);
        ret=1;
        goto _return;
    }

    project_name=strdup(basename(target_dir));
    if(!(project=gadget_project_create(project_name))) {
        xprint(ERROR, "gadget init: internal error creating project\n");
        ret=1; goto _return;
    }
    asprintf(&tmpstr,"%s/.gadget/config",target_dir);

    if(gadget_project_serialize(tmpstr,project)<0) {
        xprint(ERROR, "gadget init: cannot write project file\n");
        ret=1; goto _return;
    }

    cleanup=0;
    xprint(NORMAL,"gadget init: succesfully created new project '%s' in the folder '%s'.\n",project_name,target_dir);

_return:
    //todo cleanup freshly created directory on fail?
    //but: do not cleanup previously exiting one!
    if(tmpstr) free(tmpstr);

    if(cleanup){
        xprint(NORMAL,"gadget init: cleaning up...\n");
        if(asprintf(&tmpstr,"rm -rf %s",target_dir)>=0)
        {
            system(tmpstr);
        }
        if(tmpstr) free(tmpstr);
    }
 
    if(cmd) free(cmd);
    if(project) gadget_project_destruct(project);
    if(project_name) free(project_name);

    return 0;
}
