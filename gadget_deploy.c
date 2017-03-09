/*
 *  * Copyright (c) 2017 Cesanta Software Limited
 *   * All rights reserved
 *    */

#include "mongoose.h"
#include <stdio.h>

extern char* GADGETOSD_SERVER;
extern char* GADGETOSD_PORT;

/* RESTful server host and request URI */
const static int s_url_len=1024;
static char s_url[s_url_len];
static int s_exit_flag = 0;

static void ev_handler(struct mg_connection *nc, int ev, void *ev_data) {
  struct http_message *hm = (struct http_message *) ev_data;
  int connect_status;

  switch (ev) {
    case MG_EV_CONNECT:
      connect_status = *(int *) ev_data;
      if (connect_status != 0) {
        printf("Error connecting to %s: %s\n", s_url, strerror(connect_status));
        s_exit_flag = 1;
      }
      break;
    case MG_EV_HTTP_REPLY:
      printf("Got reply:\n%.*s\n", (int) hm->body.len, hm->body.p);
      nc->flags |= MG_F_SEND_AND_CLOSE;
      s_exit_flag = 1;
      break;
    case MG_EV_CLOSE:
      if (s_exit_flag == 0) {
        printf("Server closed connection\n");
        s_exit_flag = 1;
      };
      break;
    default:
      break;
  }
}

int gadget_deploy(int arvc,char **argv)
{

  snprintf(s_url,s_url_len,"http://%s:%s/api/v1/version",GADGETOSD_SERVER,GADGETOSD_PORT);
  struct mg_mgr mgr;
  struct mg_connection *nc;

  mg_mgr_init(&mgr, NULL);
  nc = mg_connect_http(&mgr, ev_handler, s_url, NULL, NULL);
  mg_set_protocol_http_websocket(nc);

  printf("Starting RESTful client against %s\n", s_url);
  while (s_exit_flag == 0) {
    mg_mgr_poll(&mgr, 1000);
  }
  mg_mgr_free(&mgr);

  return 0;
}
