#include "version.h"
#include "mongoose.h"

void handle_application_stop(struct mg_connection *nc, int ev, void *p)
{
    struct http_message *hm = (struct http_message *) p;

    fprintf(stderr,"handle_stop()\n");

    switch (ev) {
        case MG_EV_HTTP_REQUEST: {
            system("docker stop gadget_build_123_c");
   
            mg_printf(nc, 
               "HTTP/1.1 200 OK\r\n"
               "Content-Type: text/html\r\n"
               "Connection: close\r\n"
               "\r\n"
               "STOPPED\r\n"
               "\r\n" );
            nc->flags |= MG_F_SEND_AND_CLOSE;
            break;
        }
        default:
            fprintf(stderr, "unknown: %d\n", ev);
    }
}
