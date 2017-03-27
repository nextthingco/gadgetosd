/*
 * ex: softtabstop=4 shiftwidth=4 tabstop=4 expandtab
 *
 * Copyright (c) 2017 Next Thing Co
 * All rights reserved
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* GADGETOSD_SERVER  = "192.168.81.1";
char* GADGETOSD_PORT    = "31415";

extern int gadgetosd(int argc, char **argv);
extern int gadget(int argc, char **argv);

int main(int argc, char **argv)
{
    char* n;

    n = (n=strrchr(argv[0],'/')) ? n+1 : argv[0];

    if(getenv("GADGETOSD_SERVER")) GADGETOSD_SERVER = getenv("GADGETOSD_SERVER");
    if(getenv("GADGETOSD_PORT"))   GADGETOSD_PORT   = getenv("GADGETOSD_PORT");

    if( strcmp( n,"gadget")==0) {
        return gadget(argc,argv);
    } else {
        return gadgetosd(argc,argv);
    }
}
