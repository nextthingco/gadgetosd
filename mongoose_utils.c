#include "mongoose.h"

int mgu_get_var(struct mg_connection *nc, void *p, char *name, char* dst, size_t dst_size)
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
