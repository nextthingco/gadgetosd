/*
 * ex: softtabstop=4 shiftwidth=4 tabstop=4 expandtab
 *
 * Copyright (c) 2017 Next Thing Co
 * All rights reserved
 */

#include "mongoose.h"

#ifndef MG_ENABLE_HTTP_STREAMING_MULTIPART
#error MG_ENABLE_HTTP_STREAMING_MULTIPART not defined
#endif

struct file_writer_data {
  FILE *fp;
  size_t bytes_written;
};

void handle_docker_import(struct mg_connection *nc, int ev, void *p) {
  struct file_writer_data *data = (struct file_writer_data *) nc->user_data;
  struct mg_http_multipart_part *mp = (struct mg_http_multipart_part *) p;

  switch (ev) {
    case MG_EV_HTTP_PART_BEGIN: {
      fprintf(stderr,"MG_EV_HTTP_PART_BEGIN\n");
      if( data == NULL ) {
        data = calloc(1, sizeof(struct file_writer_data));
        //data->fp = tmpfile();
        system("docker stop gadget_build_123_c");
        system("docker rm gadget_build_123_c");
        system("docker rmi gadget_build_123");
        data->fp = popen("docker load","w");
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
      //fwrite(mp->data.p,1, mp->data.len, stderr);
      if( fwrite(mp->data.p, 1, mp->data.len, data->fp) != mp->data.len ) {
        fprintf(stderr,"FAIL\n\n");
        mg_printf(nc, "%s",
                  "HTTP/1.1 500 Failed to write to a file\r\n"
                  "Content-Length: 0\r\n\r\n");
        nc->flags |= MG_F_SEND_AND_CLOSE;
        return;
      }
      fflush(data->fp);
      data->bytes_written += mp->data.len;
      //fprintf(stderr,"MG_EV_HTTP_PART_DATA: len=%lu total=%lu\n",mp->data.len,data->bytes_written);
      break;
    }

    case MG_EV_HTTP_PART_END: {
      fprintf(stderr,"MG_EV_HTTP_PART_END\n");
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
      system("docker run --privileged -v /sys:/sys --name gadget_build_123_c gadget_build_123");
      break;
    }
    default:
        fprintf(stderr,
                "unknown: %d, MG_EV_HTTP_PART_END=%d, MG_EV_HTTP_PART_BEGIN=%d\n",
                ev, MG_EV_HTTP_PART_END, MG_EV_HTTP_PART_BEGIN
               );
  }
}
