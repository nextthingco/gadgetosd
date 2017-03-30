#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <libgen.h>
#include "utils.h"

char* GADGETOSD_SERVER  = "192.168.81.1";
char* GADGETOSD_PORT    = "31415";

char* ENDPOINT_VERSION             = "/api/version";
char* ENDPOINT_APPLICATION_ADD     = "/api/v0/application/add";
char* ENDPOINT_APPLICATION_STOP    = "/api/v0/application/stop";
char* ENDPOINT_APPLICATION_START   = "/api/v0/application/start";
char* ENDPOINT_APPLICATION_DELETE  = "/api/v0/application/delete";
char* ENDPOINT_APPLICATION_PURGE   = "/api/v0/application/purge";
char* ENDPOINT_APPLICATION_STATUS  = "/api/v0/application/status";

char* EXE_PATH                     = "initialize me!";
char* TEMPLATE_PREFIX              = "initialize me!";

#define FREE(A) if(A) { free(A); A=0; }

int initialize()
{
    if(getenv("GADGETOSD_SERVER")) GADGETOSD_SERVER = getenv("GADGETOSD_SERVER");
    if(getenv("GADGETOSD_PORT"))   GADGETOSD_PORT   = getenv("GADGETOSD_PORT");

    EXE_PATH = get_exe_path();

    asprintf(&TEMPLATE_PREFIX,"%s/share/gadget/templates",dirname(dirname(EXE_PATH)));
    if( access( TEMPLATE_PREFIX, F_OK) == -1 ) {
        // this is a hack to use templates placed next to the gadget binary
        // which is useful for development
        char *original = TEMPLATE_PREFIX;
        asprintf(&TEMPLATE_PREFIX,"%s/templates",dirname(EXE_PATH));

        if( access( TEMPLATE_PREFIX, F_OK) == -1 ) {
            fprintf(stderr, "gadget init: template prefix '%s' doesn't exist.\n", original);
            if(TEMPLATE_PREFIX) free(TEMPLATE_PREFIX);
            TEMPLATE_PREFIX=original;
            return -1;
        }
    }

    return 0;
}

void deinitialize()
{
    FREE(EXE_PATH);
    FREE(TEMPLATE_PREFIX);
}
