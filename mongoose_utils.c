#include <stdio.h>

#include "mongoose.h"
#include "gadget_project.h"
#include "config.h"

int mgu_get_var(struct mg_connection *nc, void *p, char *name, char* dst, size_t dst_size) {
    int r;
    struct http_message *hm = (struct http_message *) p;

    r=mg_get_http_var(&hm->query_string, name, dst, dst_size);

    if(r<0) {
        mg_printf(nc,
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/html\r\n"
                "Connection: close\r\n"
                "\r\n"
                "ERROR: var %s missing\r\n"
                "\r\n", name );
                nc->flags |= MG_F_SEND_AND_CLOSE;
    }
    return r;
}

static int do_rpc_exit_flag=0;

static void do_rpc_ev_handler(struct mg_connection *nc, int ev, void *ev_data) {
    struct http_message *hm = (struct http_message *) ev_data;
    int connect_status;

    switch (ev) {
        case MG_EV_CONNECT:
            connect_status = *(int *) ev_data;
            if(connect_status != 0) {
                printf("Error connecting %s\n", strerror(connect_status));
                do_rpc_exit_flag = 1;
            }
            break;
        case MG_EV_HTTP_REPLY:
            printf("Got reply:\n%.*s\n", (int) hm->body.len, hm->body.p);
            nc->flags |= MG_F_SEND_AND_CLOSE;
            do_rpc_exit_flag = 1;
            break;
        case MG_EV_CLOSE:
            if(do_rpc_exit_flag == 0) {
                printf("Server closed connection\n");
                do_rpc_exit_flag = 1;
            };
            break;
        default:
            break;
    }
}

int do_rpc(char *endpoint,gadget_project_t *project)
{
    struct mg_mgr mgr;
    struct mg_connection *nc=NULL;
    char *tmpstr=0;

    do_rpc_exit_flag=0;

    asprintf(&tmpstr,"%s?container=%s&image=%s",
             endpoint,\
             project->container_name,\
             project->container_image_name);

    mg_mgr_init(&mgr, NULL);

    nc = mg_connect_http(&mgr, do_rpc_ev_handler, URL_VERSION, NULL, NULL);
    mg_set_protocol_http_websocket(nc);
    fprintf(stderr,"requesting %s\n", URL_VERSION );
    while (do_rpc_exit_flag == 0) { mg_mgr_poll(&mgr, 1000); }
    do_rpc_exit_flag=0;

    nc = mg_connect_http(&mgr, do_rpc_ev_handler, tmpstr, NULL, NULL);
    mg_set_protocol_http_websocket(nc);
    fprintf(stderr,"requesting %s\n", tmpstr);
    while (do_rpc_exit_flag == 0) { mg_mgr_poll(&mgr, 1000); }
    do_rpc_exit_flag=0;

    mg_mgr_free(&mgr);

    if(tmpstr) free(tmpstr);
//TODO: actually return sth good here
    return 0; //SUCCESS
}
