#ifndef _CONFIG_H
#define _CONFIG_H

extern char* GADGETOSD_SERVER;
extern char* GADGETOSD_PORT;

extern char* ENDPOINT_VERSION;
extern char* ENDPOINT_APPLICATION_ADD;
extern char* ENDPOINT_APPLICATION_STOP;
extern char* ENDPOINT_APPLICATION_START;
extern char* ENDPOINT_APPLICATION_DELETE;
extern char* ENDPOINT_APPLICATION_PURGE;
extern char* ENDPOINT_APPLICATION_STATUS;

extern int initialize();

#endif
