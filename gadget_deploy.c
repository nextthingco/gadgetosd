/*
 *  Copyright (c) 2017 Next Thing Co
 *  All rights reserved
 */

#include <stdio.h>
#include "mongoose.h"
#include "utils.h"

extern char* GADGETOSD_SERVER;
extern char* GADGETOSD_PORT;

/* RESTful server host and request URI */
static const int s_url_len=1024;
static char s_url[1024];
static int s_exit_flag = 0;


extern struct mg_connection *mg_connect_http_base(
    struct mg_mgr *mgr, mg_event_handler_t ev_handler,
    struct mg_connect_opts opts, const char *schema, const char *schema_ssl,
    const char *url, const char **path, char **user, char **pass, char **addr);


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


struct mg_connection *mg_postfile_http(struct mg_mgr *mgr,
                                      mg_event_handler_t ev_handler,
                                      const char *url,
                                      const char *filename) {
    char *user = NULL, *pass = NULL, *addr = NULL;
    const char *path = NULL;
    struct mbuf auth;
    struct mg_connection *nc;  FILE* fp;
    size_t filesize;
    struct mg_connect_opts opts;
    char debug[1024*1024];
    const int buf_len=2048;
    char buf[buf_len];
    int read=0;
    struct mg_connection *ret=NULL;
    char *boundary=0;

    memset(&opts, 0, sizeof(opts));
    mbuf_init(&auth, 0);

    boundary=xuuid_generate();
    if(!boundary) { ret=NULL; goto _return; }

    fp = fopen(filename,"rb");
    if(!fp) { ret=NULL; goto _return; }
    filesize = xfsize(fp);

    nc = mg_connect_http_base(mgr, ev_handler, opts, "http://", "https://", url,
            &path, &user, &pass, &addr);
    if (nc == NULL) { ret=NULL; goto _return; }

    if (user != NULL) { mg_basic_auth_header(user, pass, &auth); }

    snprintf(debug,1024*1024,
            "POST %s HTTP/1.1\r\n"
            "Host: %s\r\n"
            "Content-Length: %" SIZE_T_FMT "\r\n"
            "Content-Type: multipart/form-data; boundary=%s\r\n"
            "%.*s\r\n"
            "\r\n"
            "--%s\r\n"
            "Content-Disposition: form-data; name=\"FILE1\"; filename=\"%s\"\r\n"
            "Content-Type: application/octet-stream\r\n"
            "Content-Transfer-Encoding: binary\r\n"
            "\r\n",
            path, addr, 2*filesize, boundary,
            (int) auth.len, (auth.buf == NULL ? "" : auth.buf),
            boundary, filename);
    fputs(debug,stderr);
    mg_printf(nc,debug);

    while(! feof(fp) ) {
        read=fread(buf,sizeof(char),buf_len,fp);
        //fwrite(buf,sizeof(char),read,stderr);
        mg_send(nc,buf,read);
    }
    mg_printf(nc,"\r\n--%s--\r\n",boundary);
    fprintf(stderr,"\r\n--%s--\r\n",boundary);
    ret=nc;

_return:
    mbuf_free(&auth);
    if(boundary) free(boundary);
    if(fp) fclose(fp);
    if(user) free(user);
    if(pass) free(pass);
    if(addr) free(addr);
    return nc;
}


int gadget_deploy(int arvc,char **argv)
{
    snprintf(s_url,s_url_len,"http://%s:%s/docker/import",GADGETOSD_SERVER,GADGETOSD_PORT);
    struct mg_mgr mgr;
    struct mg_connection *nc;
    char *filename=argv[1];

    fprintf(stderr,"sending %s...\n",filename);

    mg_mgr_init(&mgr, NULL);
    nc = mg_postfile_http(&mgr, ev_handler, s_url, filename);
    mg_set_protocol_http_websocket(nc);

    printf("Starting RESTful client against %s\n", s_url);
    while (s_exit_flag == 0) {
        mg_mgr_poll(&mgr, 1000);
    }
    mg_mgr_free(&mgr);

    return 0;
}
