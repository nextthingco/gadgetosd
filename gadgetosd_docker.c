#include "mongoose.h"

struct file_writer_data {
  FILE *fp;
  size_t bytes_written;
};

void handle_docker_import(struct mg_connection *nc, int ev, void *p) {
  struct file_writer_data *data = (struct file_writer_data *) nc->user_data;
  struct mg_http_multipart_part *mp = (struct mg_http_multipart_part *) p;

  switch (ev) {
    case MG_EV_HTTP_PART_BEGIN: {
      //fprintf(stderr,"MG_EV_HTTP_PART_BEGIN\n");
      if (data == NULL) {
        data = calloc(1, sizeof(struct file_writer_data));
        //data->fp = tmpfile();
        data->fp = popen("/usr/bin/docker import - testme","w");
        data->bytes_written = 0;

        if (data->fp == NULL) {
          mg_printf(nc, "%s",
                    "HTTP/1.1 500 Failed to open a file\r\n"
                    "Content-Length: 0\r\n\r\n");
          nc->flags |= MG_F_SEND_AND_CLOSE;
          return;
        }
        nc->user_data = (void *) data;
      }
      break;
    }
    case MG_EV_HTTP_PART_DATA: {
      //fprintf(stderr,"MG_EV_HTTP_PART_DATA: len=%d\n",mp->data.len);
      //fwrite(mp->data.p,1, mp->data.len, stderr);
      if (fwrite(mp->data.p, 1, mp->data.len, data->fp) != mp->data.len) {
        fprintf(stderr,"FAIL\n\n");
        mg_printf(nc, "%s",
                  "HTTP/1.1 500 Failed to write to a file\r\n"
                  "Content-Length: 0\r\n\r\n");
        nc->flags |= MG_F_SEND_AND_CLOSE;
        return;
      }
      fflush(data->fp);
      data->bytes_written += mp->data.len;
      break;
    }
    case MG_EV_HTTP_PART_END: {
      //fprintf(stderr,"MG_EV_HTTP_PART_END\n");
      int ret=WEXITSTATUS(pclose(data->fp));
      if(ret) {
          mg_printf(nc,
                  "HTTP/1.1 400 ERROR\r\n"
                  "Content-Type: text/plain\r\n"
                  "Connection: close\r\n\r\n"
                  "ERROR\n\n" );
      } else {
          mg_printf(nc,
                  "HTTP/1.1 200 OK\r\n"
                  "Content-Type: text/plain\r\n"
                  "Connection: close\r\n\r\n"
                  "SUCCESS\n\n" );
      }
      nc->flags |= MG_F_SEND_AND_CLOSE;
      free(data);
      nc->user_data = NULL;
      break;
    }
    default:
        fprintf(stderr,"unknown: %d\n",ev);
  }
}
