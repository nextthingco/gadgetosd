#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <libgen.h>
#include <string.h>
#include "utils.h"

#ifndef __CYGWIN__
char* GADGETOSD_SERVER  = "192.168.81.1";
#else
char* GADGETOSD_SERVER  = "192.168.82.1";
#endif
char* GADGETOSD_PORT    = "31415";

char* ENDPOINT_VERSION             = "/api/version";
char* ENDPOINT_APPLICATION_ADD     = "/api/v0/application/add";
char* ENDPOINT_APPLICATION_CREATE  = "/api/v0/application/create";
char* ENDPOINT_APPLICATION_START   = "/api/v0/application/start";
char* ENDPOINT_APPLICATION_STOP    = "/api/v0/application/stop";
char* ENDPOINT_APPLICATION_DELETE  = "/api/v0/application/delete";
char* ENDPOINT_APPLICATION_PURGE   = "/api/v0/application/purge";
char* ENDPOINT_APPLICATION_STATUS  = "/api/v0/application/status";

char* EXE_PATH                     = "initialize me!";
char* TEMPLATE_PREFIX              = "initialize me!";
char* GADGET_CONFIG_FILE           = "gadget.cfg";

int   _VERBOSE                     = 0;

#define FREE(A) if(A) { free(A); A=0; }

int initialize()
{
    int ret=0;
    char *original=0;
    char *fallback=0;

    if(getenv("GADGETOSD_SERVER")) GADGETOSD_SERVER = getenv("GADGETOSD_SERVER");
    if(getenv("GADGETOSD_PORT"))   GADGETOSD_PORT   = getenv("GADGETOSD_PORT");

    EXE_PATH = get_exe_path();
    original = (char*)strdup(EXE_PATH);

    asprintf(&TEMPLATE_PREFIX,"%s/share/gadget/templates",dirname(dirname(original)));
    if( access( TEMPLATE_PREFIX, F_OK) == -1 ) {
        // this is a hack to use templates placed next to the gadget binary
        // which is useful for development
        fallback = (char*)strdup(EXE_PATH);
        asprintf(&TEMPLATE_PREFIX,"%s/templates",dirname(fallback));
        //xprint(ERROR,"trying '%s'\n",TEMPLATE_PREFIX);

        if( access( TEMPLATE_PREFIX, F_OK) == -1 ) {
            xprint(ERROR, "gadget init: template prefix '%s' doesn't exist.\n", original);
            if(TEMPLATE_PREFIX) free(TEMPLATE_PREFIX);
            TEMPLATE_PREFIX=original;
            ret=-1;
            goto _return;
        }
    }

_return:
    if(fallback) free(fallback);
    if(original) free(original);
    return ret;
}

void deinitialize()
{
    FREE(EXE_PATH);
    FREE(TEMPLATE_PREFIX);
}
