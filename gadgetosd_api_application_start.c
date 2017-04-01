#include "version.h"
#include "mongoose.h"
#include "mongoose_utils.h"

void handle_application_start(struct mg_connection *nc, int ev, void *p)
{
    char container_name[4096];
    char container_image_name[4096];
    char cmd[4096];
    int r=0;

    fprintf(stderr,"handle_start()\n");

    switch (ev) {
        case MG_EV_HTTP_REQUEST: {
            r=mgu_get_var(nc,p,"container",container_name, sizeof(container_name));
            if(r<0) break;
            r=mgu_get_var(nc,p,"image", container_image_name, sizeof(container_image_name));
            if(r<0) break;

            snprintf(cmd,sizeof(cmd),"docker start %s",container_name);
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
