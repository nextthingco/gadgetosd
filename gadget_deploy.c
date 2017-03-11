/*
 *  * Copyright (c) 2017 Cesanta Software Limited
 *   * All rights reserved
 *    */

#include "mongoose.h"
#include <stdio.h>
#include <uuid/uuid.h>

extern char* GADGETOSD_SERVER;
extern char* GADGETOSD_PORT;

/* RESTful server host and request URI */
const static int s_url_len=1024;
static char s_url[s_url_len];
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

int encode_chunk(char *boundary, char *name, const char *filename, FILE* fp, int chunksize, char **buf, int *buf_len)
{
    static char* format=
"--%s\r\n"
"Content-Disposition: form-data; name=\"%s\"; filename=\"%s\"\r\n"
"Content-Type: application/octet-stream\r\n"
"Content-Transfer-Encoding: binary\r\n"
"\r\n";

    int len = strlen(boundary)
            + strlen(name)
            + strlen(filename)
            + strlen(format)
            + chunksize
            + 4    //for trailing \r\n\r\n
            + 1;   //for termination \0

    int pos = 0;
    size_t read = 0;

    *buf=realloc(*buf,sizeof(char)*len);

    snprintf(*buf,len,format,boundary,name,filename);
    pos = strlen(*buf); 
    
    read=fread((*buf)+pos,sizeof(char),chunksize,fp);
    //fprintf(stderr,"read %lu bytes from %s\n",read,filename);
    snprintf((*buf)+pos+read,3,"\r\n");
    *buf_len = pos+read+2;

    return 0;
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
  uuid_t uuid;
  char boundary[33];
  char debug[1024*1024];
  char *buf = 0;
  int buf_len;

  memset(&opts, 0, sizeof(opts));

  uuid_generate(uuid);
  for(int i=0;i<16;i++) { snprintf(boundary+(i*2),3,"%02x",uuid[i]); }

  fp = fopen(filename,"rb");
  if(!fp) return NULL;
  fseek(fp, 0, SEEK_END);
  filesize = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  nc = mg_connect_http_base(mgr, ev_handler, opts, "http://", "https://", url,
                           &path, &user, &pass, &addr);

  if (nc == NULL) {
    fclose(fp);
    return NULL;
  }

  mbuf_init(&auth, 0);
  if (user != NULL) {
    mg_basic_auth_header(user, pass, &auth);
  }
 
  snprintf(debug,1024*1024,
                "POST %s HTTP/1.1\r\n"
                "Host: %s\r\n"
                "Content-Length: %" SIZE_T_FMT "\r\n"
                "Content-Type: multipart/form-data; boundary=%s\r\n"
                "%.*s\r\n\r\n",
            path, addr, 2*filesize, boundary,
            (int) auth.len, (auth.buf == NULL ? "" : auth.buf) );
  fputs(debug,stderr);

  mg_printf(nc,debug);

  while(! feof(fp) ) {
      encode_chunk(boundary,"FILE1",filename,fp,1024,&buf,&buf_len);
      //fprintf(stderr,"[[[strlen(buf)=%lu]]]\n",strlen(buf));
      fputs(buf,stderr);
      fprintf(stderr,"[[[sending %d bytes]]]\n",buf_len);
      mg_send(nc,buf,buf_len);
  }
  mg_printf(nc,"--%s--\r\n",boundary);
  fprintf(stderr,"--%s--\r\n",boundary);
  free(buf);

  fclose(fp);
  mbuf_free(&auth);
  free(user);
  free(pass);
  free(addr);
  return nc;
}


int gadget_deploy(int arvc,char **argv)
{
    snprintf(s_url,s_url_len,"http://%s:%s/upload",GADGETOSD_SERVER,GADGETOSD_PORT);
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
