// Copyright (c) 2017 Next Thing Co
// All rights reserved

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mongoose.h"
#include "gadgetosd_docker.h"

#ifndef MG_ENABLE_HTTP_STREAMING_MULTIPART
#error MG_ENABLE_HTTP_STREAMING_MULTIPART not defined
#endif

extern char *GADGETOSD_PORT;


struct file_writer_data {
  FILE *fp;
  size_t bytes_written;
};


static void handle_request(struct mg_connection *nc) {
  mg_printf(nc, "%s",
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Connection: close\r\n"
            "\r\n"
            "<html><body>Hello!</body></html>" );
  nc->flags |= MG_F_SEND_AND_CLOSE;
}


static void handle_upload(struct mg_connection *nc, int ev, void *p) {
  struct file_writer_data *data = (struct file_writer_data *) nc->user_data;
  struct mg_http_multipart_part *mp = (struct mg_http_multipart_part *) p;

  switch (ev) {
    case MG_EV_HTTP_PART_BEGIN: {
      //fprintf(stderr,"MG_EV_HTTP_PART_BEGIN\n");
      if (data == NULL) {
        data = calloc(1, sizeof(struct file_writer_data));
        //data->fp = tmpfile();
        data->fp = fopen("tmpfile","wb");
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
      mg_printf(nc,
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/plain\r\n"
                "Connection: close\r\n\r\n"
                "Written %ld of POST data to a temp file\n\n",
                (long) ftell(data->fp));
      nc->flags |= MG_F_SEND_AND_CLOSE;
      fclose(data->fp);
      free(data);
      nc->user_data = NULL;
      break;
    }
    default:
        fprintf(stderr,"unknown: %d\n",ev);
  }
}



static void ev_handler(struct mg_connection *nc, int ev, void *ev_data) {
  (void) ev_data;
  switch (ev) {
    case MG_EV_HTTP_REQUEST:
      // Invoked when the full HTTP request is in the buffer (including body).
      handle_request(nc);
      break;
  }
}


int gadgetosd(int argc, char **argv) {
  struct mg_mgr mgr;
  struct mg_connection *nc;

  mg_mgr_init(&mgr, NULL);
  nc = mg_bind(&mgr, GADGETOSD_PORT, ev_handler);

  mg_register_http_endpoint(nc, "/upload", handle_upload);
  mg_register_http_endpoint(nc, "/docker/import", handle_docker_import);
  // Set up HTTP server parameters
  mg_set_protocol_http_websocket(nc);

  printf("gadgetosd running on port %s\n", GADGETOSD_PORT);

  while(1) {
      mg_mgr_poll(&mgr, 1000);
  }
  mg_mgr_free(&mgr);

  return 0;
}
