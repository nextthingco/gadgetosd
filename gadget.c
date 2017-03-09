/*
 * Copyright (c) 2017 Next Thing Co
 * All rights reserved
 */

#include <stdio.h>
#include <string.h>

#include "version.h"
#include "mongoose.h"

typedef int (*funcptr)(int,char **);

typedef struct command {
    char     *name;
    funcptr function;
} command;

int gadget_help(int argc, char **argv);
extern int gadget_new(int argc, char **argv);
extern int gadget_build(int argc, char **argv);
extern int gadget_deploy(int argc, char **argv);

command commands[] = {
    { "--help", gadget_help },
    { "-h", gadget_help },
    { "-?", gadget_help },
    { "help", gadget_help },
    { "new", gadget_new },
    { "build", gadget_build },
    { "deploy", gadget_deploy },
    { 0, 0 }
};


int gadget(int argc, char **argv)
{
    int i;

    if(argc<2) {
        return gadget_help(argc,argv);
    }

    for(i=0; commands[i].name; i++) {
        if(!strcmp(argv[1],commands[i].name))
            return commands[i].function(argc,argv);
    }

    printf("gadget: '%s' is not a gadget command. Try %s --help.\n",argv[1],argv[0]);
}

int gadget_help(int argc, char **argv)
{
    printf(
"Create embbeded Linux apps - easy.\n"
"\n"
"usage: gadget <command> [<args>]\n"
"\n"
"common gadget commands: \n"
"  new         Create an empty gadget project \n"
"  build       Build the project in the current directory\n"
"  deploy      Deploys the project in the current directory\n"
"\n"
"optional arguments:\n"
"  -h, --help  show this help message and exit\n"
"\n"
"get help for specific command:\n"
"  gadget <command> --help\n"
);

    return 0;
}
