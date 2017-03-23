#include "version.h"
#include "mongoose.h"

int get_var(struct mg_connection *nc, void *p, char *name, char* dst, size_t dst_size)
{
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


void handle_application_start(struct mg_connection *nc, int ev, void *p)
{
    char container_name[4096];
    char container_image_name[4096];
    char cmd[4096];
    int r=0;

    fprintf(stderr,"handle_start()\n");

    switch (ev) {
        case MG_EV_HTTP_REQUEST: {
            r=get_var(nc,p,"container",container_name, sizeof(container_name));
            if(r<0) break;
            r=get_var(nc,p,"image", container_image_name, sizeof(container_image_name));
            if(r<0) break;

            snprintf(cmd,sizeof(cmd),"docker run --privileged -v /sys:/sys --name %s %s&",container_name,container_image_name);
            fprintf(stderr,"running '%s'\n",cmd);
            if(system(cmd)) {
                mg_printf(nc,
                        "HTTP/1.1 200 OK\r\n"
                        "Content-Type: text/html\r\n"
                        "Connection: close\r\n"
                        "\r\n"
                        "ERROR running %s\r\n"
                        "\r\n",cmd );
                nc->flags |= MG_F_SEND_AND_CLOSE;
            } else {
                mg_printf(nc,
                        "HTTP/1.1 200 OK\r\n"
                        "Content-Type: text/html\r\n"
                        "Connection: close\r\n"
                        "\r\n"
                        "OK\r\n"
                        "\r\n" );
                nc->flags |= MG_F_SEND_AND_CLOSE;
            }
            break;
        }
        default:
            fprintf(stderr, "unknown: %d\n", ev);
    }
}
