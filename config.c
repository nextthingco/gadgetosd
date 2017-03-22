#include <stdlib.h>
#include <stdio.h>

char* GADGETOSD_SERVER  = "192.168.81.1";
char* GADGETOSD_PORT    = "31415";

char* ENDPOINT_VERSION             = "/api/version";
char* ENDPOINT_APPLICATION_ADD     = "/api/v0/application/add";
char* ENDPOINT_APPLICATION_STOP    = "/api/v0/application/stop";

char* URL_VERSION             = 0;
char* URL_APPLICATION_ADD     = 0;
char* URL_APPLICATION_STOP    = 0;

#define FREE(A) if(A) { free(A); A=0; }

int initialize()
{
    int ret=0;

    if(getenv("GADGETOSD_SERVER")) GADGETOSD_SERVER = getenv("GADGETOSD_SERVER");
    if(getenv("GADGETOSD_PORT"))   GADGETOSD_PORT   = getenv("GADGETOSD_PORT");

    if( asprintf(&URL_VERSION, "http://%s:%s%s", GADGETOSD_SERVER, GADGETOSD_PORT, ENDPOINT_VERSION) < 0 ) { ret=1; goto _error; }
    if( asprintf(&URL_APPLICATION_ADD, "http://%s:%s%s", GADGETOSD_SERVER, GADGETOSD_PORT, ENDPOINT_APPLICATION_ADD) < 0 ) { ret=1; goto _error; }
    if( asprintf(&URL_APPLICATION_STOP, "http://%s:%s%s", GADGETOSD_SERVER, GADGETOSD_PORT, ENDPOINT_APPLICATION_STOP) < 0 ) { ret=1; goto _error; }

    goto _return;
_error:
    ret=-1;
    FREE(URL_VERSION);
    FREE(URL_APPLICATION_ADD);
    FREE(URL_APPLICATION_STOP);

_return:
    return ret;
}

int deinitialize()
{
    FREE(URL_VERSION);
    FREE(URL_APPLICATION_ADD);
    FREE(URL_APPLICATION_STOP);

    return 0; //SUCCESS
}
