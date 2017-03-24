/*
 * ex: softtabstop=4 shiftwidth=4 tabstop=4 expandtab
 *
 * Copyright (c) 2017 Next Thing Co
 * All rights reserved
 */

#include <stdio.h>
#include <string.h>

#include "config.h"
#include "version.h"
#include "mongoose.h"
#include "utils.h"

typedef int (*funcptr)(int,char **);

typedef struct command {
    char     *name;
    funcptr function;
} command;

int gadget_help(int argc, char **argv);
extern int gadget_init(int argc, char **argv);
extern int gadget_build(int argc, char **argv);
extern int gadget_deploy(int argc, char **argv);
extern int gadget_stop(int argc, char **argv);
extern int gadget_start(int argc, char **argv);
extern int gadget_delete(int argc, char **argv);
extern int gadget_purge(int argc, char **argv);
extern int gadget_status(int argc, char **argv);

command commands[] = {
    { "--help", gadget_help },
    { "-h",     gadget_help },
    { "-?",     gadget_help },
    { "help",   gadget_help },
    { "init",   gadget_init },
    { "build",  gadget_build },
    { "deploy", gadget_deploy },
    { "stop",   gadget_stop },
    { "start",  gadget_start },
    { "delete", gadget_delete },
    { "purge",  gadget_purge },
    { 0, 0 }
};


int main(int argc, char **argv)
{
    int i=0,ret=0;

    initialize();

    if(argc<2) {
        return gadget_help(argc,argv);
    }

    for(i=0; commands[i].name; i++) {
        if(!strcmp(argv[1],commands[i].name)) {
            ret=commands[i].function(argc-1,&argv[1]);
            goto _return;
        }
    }

    printf("gadget: '%s' is not a gadget command. Try %s --help.\n",argv[1],argv[0]);
    ret = 1;

_return:
    deinitialize();
    return ret;
}

int gadget_help(int argc, char **argv)
{
    printf(
"Create embedded Linux apps - easy.\n"
"\n"
"usage: gadget <command> [<args>]\n"
"\n"
"common gadget commands: \n"
"  init        Create an empty gadget project \n"
"  build       Build the project in the current directory\n"
"  deploy      Deploys the project in the current directory\n"
"  start       Starts the application on a device\n"
"  stop        Stops the application running on a device\n"
"  delete      Deletes the application instance from a device\n"
"  purge       Purges the application from a device\n"
"  status      Shows status information\n"
"\n"
"optional arguments:\n"
"  -h, --help  show this help message and exit\n"
"\n"
"get help for specific command:\n"
"  gadget <command> --help\n"
);

    return 0;
}
