/*
 * ex: softtabstop=4 shiftwidth=4 tabstop=4 expandtab
 *
 * Copyright (c) 2017 Next Thing Co
 * All rights reserved
 */

#ifndef MG_ENABLE_HTTP_STREAMING_MULTIPART
#error MG_ENABLE_HTTP_STREAMING_MULTIPART not defined
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mongoose.h"
#include "config.h"

extern void handle_version(struct mg_connection *nc, int ev, void *p);
extern void handle_application_add(struct mg_connection *nc, int ev, void *p);
extern void handle_application_stop(struct mg_connection *nc, int ev, void *p);
extern void handle_application_start(struct mg_connection *nc, int ev, void *p);
extern void handle_application_delete(struct mg_connection *nc, int ev, void *p);
extern void handle_application_purge(struct mg_connection *nc, int ev, void *p);

extern void mg_http_handler(struct mg_connection *nc, int ev, void *ev_data);

static void ev_handler(struct mg_connection *nc, int ev, void *ev_data)
{
    struct http_message *hm = (struct http_message *) ev_data;

    switch (ev) {
        case MG_EV_HTTP_REQUEST:
            // Invoked when the full HTTP request is in the buffer (including body).
            mg_printf(nc, 
                    "HTTP/1.1 200 OK\r\n"
                    "Content-Type: text/html\r\n"
                    "Connection: close\r\n"
                    "\r\n"
                    "ERROR: unsupported endpoint: %.*s",(int)hm->uri.len,hm->uri.p); 
            nc->flags |= MG_F_SEND_AND_CLOSE;
            break;
    }
}

int main(int argc, char **argv)
{
    int ret=0;
    struct mg_mgr mgr;
    struct mg_connection *nc;

    initialize();

    mg_mgr_init(&mgr, NULL);
    nc = mg_bind(&mgr, GADGETOSD_PORT, ev_handler);
    if(!nc) { 
        fprintf(stderr,"gadgetosd: ERROR: cannot create network connection\n");
        ret=1; goto _return;
    }

    mg_register_http_endpoint(nc, ENDPOINT_VERSION,            handle_version);
    mg_register_http_endpoint(nc, ENDPOINT_APPLICATION_ADD,    handle_application_add);
    mg_register_http_endpoint(nc, ENDPOINT_APPLICATION_STOP,   handle_application_stop);
    mg_register_http_endpoint(nc, ENDPOINT_APPLICATION_START,  handle_application_start);
    mg_register_http_endpoint(nc, ENDPOINT_APPLICATION_DELETE, handle_application_delete);
    mg_register_http_endpoint(nc, ENDPOINT_APPLICATION_PURGE,  handle_application_purge);
    mg_set_protocol_http_websocket(nc);

    fprintf(stderr,"gadgetosd running on port %s\n", GADGETOSD_PORT);

    while(1) {
        mg_mgr_poll(&mgr, 1000);
    }
    mg_mgr_free(&mgr);

_return:
    deinitialize();
    return ret;
}
