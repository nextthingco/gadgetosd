#include "version.h"
#include "mongoose.h"

void handle_version(struct mg_connection *nc, int ev, void *p) {
    switch (ev) {
        case MG_EV_HTTP_REQUEST: {
            mg_printf(nc, 
               "HTTP/1.1 200 OK\r\n"
               "Content-Type: text/html\r\n"
               "Connection: close\r\n"
               "\r\n"
               "%d.%d.%d\r\n"
               "\r\n",
               GADGETOSD_MAJOR,
               GADGETOSD_MINOR,
               GADGETOSD_PATCH );
            nc->flags |= MG_F_SEND_AND_CLOSE;
            break;
        }
        default:
            fprintf(stderr, "unknown: %d\n", ev);
    }
}
