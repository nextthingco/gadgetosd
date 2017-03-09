/*
 * Copyright (c) 2017 Next Thing Co
 * All rights reserved
 */


#include "version.h"
#include "mongoose.h"

extern char *GADGETOSD_PORT;

static void ev_handler(struct mg_connection *nc, int ev, void *ev_data) {
  struct http_message *hm = (struct http_message *) ev_data;

  switch (ev) {
    case MG_EV_HTTP_REQUEST:
      if (mg_vcmp(&hm->uri, "/api/v1/version") == 0) {
        mg_printf(nc, "%s", "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");
        mg_printf_http_chunk(nc, "VERSION: %d.%d.%d\r\n\r\n", GADGETOSD_MAJOR, GADGETOSD_MINOR, GADGETOSD_BUGFIX);
        mg_send_http_chunk(nc, "", 0); /* Send empty chunk, the end of response */
      } else if (mg_vcmp(&hm->uri, "/api/v1/docker/import") == 0) {
        mg_printf(nc, "%s", "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");
        mg_printf_http_chunk(nc, "SHUTDOWN!\r\n\r\n");
        mg_send_http_chunk(nc, "", 0); /* Send empty chunk, the end of response */
      } else { 
        mg_printf(nc, "%s", "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");
        mg_printf_http_chunk(nc, "HELLO\r\n\r\n");
        mg_send_http_chunk(nc, "", 0); /* Send empty chunk, the end of response */
      }
      break;
    default:
      break;
  }
}

int gadgetosd(int argc, char *argv[]) {
  struct mg_mgr mgr;
  struct mg_connection *nc;
  struct mg_bind_opts bind_opts;

  int i;
  char *cp;
  const char *err_str;
#if MG_ENABLE_SSL
  const char *ssl_cert = NULL;
#endif

  mg_mgr_init(&mgr, NULL);

  /* Set HTTP server options */
  memset(&bind_opts, 0, sizeof(bind_opts));
  bind_opts.error_string = &err_str;
#if MG_ENABLE_SSL
  if (ssl_cert != NULL) {
    bind_opts.ssl_cert = ssl_cert;
  }
#endif

  nc = mg_bind_opt(&mgr, GADGETOSD_PORT, ev_handler, bind_opts);
  if (nc == NULL) {
    fprintf(stderr, "Error starting server on port %s: %s\n", GADGETOSD_PORT,
            *bind_opts.error_string);
    exit(1);
  }

  mg_set_protocol_http_websocket(nc);

  fprintf(stderr,"GadgetOSD %d.%d.%d on port %s\n\n", GADGETOSD_MAJOR, GADGETOSD_MINOR, GADGETOSD_BUGFIX, GADGETOSD_PORT);

  while (1) {
    mg_mgr_poll(&mgr, 1000);
  }
  mg_mgr_free(&mgr);

  return 0;
}
