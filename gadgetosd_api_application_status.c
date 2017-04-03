#include "version.h"
#include "mongoose.h"
#include "mongoose_utils.h"
#include "utils.h"

void handle_application_status(struct mg_connection *nc, int ev, void *p)
{
    char cmd[4096];
    char *output_docker_ps_a=0;
    char *output_docker_images=0;
    char *output_df_h=0;
    int  r=0;

    fprintf(stderr,"handle_delete()\n");

    switch (ev) {
        case MG_EV_HTTP_REQUEST: {
            snprintf(cmd,sizeof(cmd),"docker ps -a");
            fprintf(stderr,"running '%s'\n",cmd);
            r |= xrun("docker ps -a",&output_docker_ps_a);
            r |= xrun("docker images",&output_docker_images);
            r |= xrun("df -h",&output_df_h);
            if(r) {
                mg_printf(nc,
                        "HTTP/1.1 200 OK\r\n"
                        "Content-Type: text/html\r\n"
                        "Connection: close\r\n"
                        "\r\n"
                        "ERROR:\r\n"
                        "%s\r\n"
                        "\r\n"
                        "%s\r\n"
                        "\r\n"
                        "%s\r\n"
                        "\r\n", output_docker_ps_a, output_docker_images, output_df_h );
                nc->flags |= MG_F_SEND_AND_CLOSE;
            } else {
                mg_printf(nc,
                        "HTTP/1.1 200 OK\r\n"
                        "Content-Type: text/html\r\n"
                        "Connection: close\r\n"
                        "\r\n"
                        "%s\r\n"
                        "\r\n"
                        "%s\r\n"
                        "\r\n"
                        "%s\r\n"
                        "\r\n"
                        "\r\n", output_docker_ps_a, output_docker_images, output_df_h );
                nc->flags |= MG_F_SEND_AND_CLOSE;
            }
            break;
        }
        default:
            fprintf(stderr, "unknown: %d\n", ev);
    }

    if(output_docker_ps_a)   free(output_docker_ps_a);
    if(output_docker_images) free(output_docker_images);
    if(output_df_h)          free(output_df_h);
}
