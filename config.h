#ifndef _CONFIG_H
#define _CONFIG_H

extern char* GADGETOSD_SERVER;
extern char* GADGETOSD_PORT;

extern char* ENDPOINT_VERSION;
extern char* ENDPOINT_APPLICATION_ADD;
extern char* ENDPOINT_APPLICATION_CREATE;
extern char* ENDPOINT_APPLICATION_START;
extern char* ENDPOINT_APPLICATION_STOP;
extern char* ENDPOINT_APPLICATION_DELETE;
extern char* ENDPOINT_APPLICATION_PURGE;
extern char* ENDPOINT_APPLICATION_STATUS;

extern char* EXE_PATH;
extern char* TEMPLATE_PREFIX;

extern int _VERBOSE;

extern int initialize();
extern void deinitialize();

#endif
