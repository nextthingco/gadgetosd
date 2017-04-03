#include <stdio.h>

#include "config.h"
#include "utils.h"
#include "gadget_project.h"
#include "mongoose.h"

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

//------------------------------------------------------------------------------

char *build_url( char* endpoint, gadget_project_t *project)
{
    char *tmpstr=0;

    if(project) {
        if( asprintf(&tmpstr,
                     "http://%s:%s%s?container=%s&image=%s",
                     GADGETOSD_SERVER,
                     GADGETOSD_PORT,
                     endpoint,
                     project->container_name,
                     project->container_image_name
                    ) < 0
          )
            return 0;
    } else {
        if( asprintf(&tmpstr,
                     "http://%s:%s%s",
                     GADGETOSD_SERVER,
                     GADGETOSD_PORT,
                     endpoint
                    ) < 0
          )
            return 0;
    }

    return tmpstr;
}

//------------------------------------------------------------------------------

static int do_rpc_exit_flag=0;

static void do_rpc_ev_handler(struct mg_connection *nc, int ev, void *ev_data)
{
    struct http_message *hm = (struct http_message *) ev_data;
    int connect_status;

    switch (ev) {
        case MG_EV_CONNECT:
            connect_status = *(int *) ev_data;
            if(connect_status != 0) {
                xprint(ERROR,"Error connecting %s\n", strerror(connect_status));
                do_rpc_exit_flag = 1;
            }
            break;
        case MG_EV_HTTP_REPLY:
            xprint(VERBOSE,"Got reply:\n%.*s\n", (int) hm->body.len, hm->body.p);
            nc->flags |= MG_F_SEND_AND_CLOSE;
            do_rpc_exit_flag = 1;
            break;
        case MG_EV_CLOSE:
            if(do_rpc_exit_flag == 0) {
                xprint(ERROR,"Server closed connection\n");
                do_rpc_exit_flag = 1;
            };
            break;
        default:
            break;
    }
}

int do_rpc(char *endpoint,gadget_project_t *project)
{
    int ret=0;
    struct mg_mgr mgr;
    struct mg_connection *nc=NULL;
    char *tmpstr=0;

    do_rpc_exit_flag=0;
    mg_mgr_init(&mgr, NULL);

    if(!(tmpstr = build_url(endpoint,project))) {
        ret=-1;
        goto _return;
    }
    nc = mg_connect_http(&mgr, do_rpc_ev_handler, tmpstr, NULL, NULL);
    mg_set_protocol_http_websocket(nc);
    xprint(VERBOSE,"requesting %s\n", tmpstr);
    while (do_rpc_exit_flag == 0) { mg_mgr_poll(&mgr, 1000); }
    do_rpc_exit_flag=0;

_return:
    mg_mgr_free(&mgr);
    if(tmpstr) free(tmpstr);
    return ret;
}

//------------------------------------------------------------------------------
