#include <stdlib.h>
#include <stdio.h>

char* GADGETOSD_SERVER  = "192.168.81.1";
char* GADGETOSD_PORT    = "31415";

char* ENDPOINT_VERSION             = "/api/version";
char* ENDPOINT_APPLICATION_ADD     = "/api/v0/application/add";
char* ENDPOINT_APPLICATION_STOP    = "/api/v0/application/stop";
char* ENDPOINT_APPLICATION_START   = "/api/v0/application/start";
char* ENDPOINT_APPLICATION_DELETE  = "/api/v0/application/delete";
char* ENDPOINT_APPLICATION_PURGE   = "/api/v0/application/purge";
char* ENDPOINT_APPLICATION_STATUS  = "/api/v0/application/status";

#define FREE(A) if(A) { free(A); A=0; }

int initialize()
{
    if(getenv("GADGETOSD_SERVER")) GADGETOSD_SERVER = getenv("GADGETOSD_SERVER");
    if(getenv("GADGETOSD_PORT"))   GADGETOSD_PORT   = getenv("GADGETOSD_PORT");

    return 0;
}
