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

extern char* URL_VERSION;
extern char* URL_APPLICATION_ADD;
extern char* URL_APPLICATION_STOP;
extern char* URL_APPLICATION_START;
extern char* URL_APPLICATION_DELETE;
extern char* URL_APPLICATION_PURGE;
extern char* URL_APPLICATION_STATUS;

extern int initialize();
extern int deinitialize();

#endif
