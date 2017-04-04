/*
 * ex: softtabstop=4 shiftwidth=4 tabstop=4 expandtab
 *
 * Copyright (c) 2017 Next Thing Co
 * All rights reserved
 */

#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include "utils.h"
#include "config.h"
#include "gadget_project.h"
#include "mongoose.h"
#include "mongoose_utils.h"

/* RESTful server host and request URI */
static int s_exit_flag = 0;

extern struct mg_connection *mg_connect_http_base(
    struct mg_mgr *mgr, mg_event_handler_t ev_handler,
    struct mg_connect_opts opts, const char *schema, const char *schema_ssl,
    const char *url, const char **path, char **user, char **pass, char **addr);

//-- BEGIN: work around weak declaratioon problem on windows
// we redfine mbuf_init(), mbuf_resize() and mbuf_free() here
void mbuf_init(struct mbuf *mbuf, size_t initial_size) {
  mbuf->len = mbuf->size = 0;
  mbuf->buf = NULL;
  mbuf_resize(mbuf, initial_size);
}

void mbuf_free(struct mbuf *mbuf) {
  if (mbuf->buf != NULL) {
    free(mbuf->buf);
    mbuf_init(mbuf, 0);
  }
}

void mbuf_resize(struct mbuf *a, size_t new_size) {
  if (new_size > a->size || (new_size < a->size && new_size >= a->len)) {
    char *buf = (char *) realloc(a->buf, new_size);
    /*
     * In case realloc fails, there's not much we can do, except keep things as
     * they are. Note that NULL is a valid return value from realloc when
     * size == 0, but that is covered too.
     */
    if (buf == NULL && new_size != 0) return;
    a->buf = buf;
    a->size = new_size;
  }
}
//-- END: work around weak declaratioon problem on windows

static void ev_handler(struct mg_connection *nc, int ev, void *ev_data) {
    struct http_message *hm = (struct http_message *) ev_data;
    int connect_status;

    switch (ev) {
        case MG_EV_CONNECT:
            connect_status = *(int *) ev_data;
            if (connect_status != 0) {
                xprint(ERROR,"Error connecting %s\n", strerror(connect_status));
                s_exit_flag = 1;
            }
            break;
        case MG_EV_HTTP_REPLY:
            xprint(VERBOSE,"Got reply:\n%.*s\n", (int) hm->body.len, hm->body.p);
            nc->flags |= MG_F_SEND_AND_CLOSE;
            s_exit_flag = 1;
            break;
        case MG_EV_CLOSE:
            if (s_exit_flag == 0) {
                xprint(NORMAL,"Server closed connection\n");
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
    struct mg_connection *nc=NULL;
    FILE* fp=NULL;
    size_t filesize;
    struct mg_connect_opts opts;
    char *header=0;
    const int buf_len=4096;
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

    asprintf(&header,
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
    //fputs(header,stderr);
    mg_printf(nc,"%s",header);

    while(! feof(fp) ) {
        read=fread(buf,sizeof(char),buf_len,fp);
        if(read<0) {
            xprint(ERROR,"ERROR: while reading file");
        }
        mg_send(nc,buf,read);
        mg_mgr_poll(mgr, 1);
    }
    mg_printf(nc,"\r\n--%s--\r\n",boundary);
    //fprintf(stderr,"\r\n--%s--\r\n",boundary);
    ret=nc;

_return:
    mbuf_free(&auth);
    if(boundary) free(boundary);
    if(fp) fclose(fp);
    if(user) free(user);
    if(pass) free(pass);
    if(addr) free(addr);
    if(header) free(header);
    return ret;
}

void gadget_deploy_help()
{
    xprint( NORMAL,
            "Create embedded Linux apps - easy.\n"
            "\n"
            "usage: gadget deploy [<project_path>]\n"
            "\n"
            "optional arguments:\n"
            "  -h, --help            show this help message and exit\n"
            "  -v, --verbose         be verbose\n"
            "  <project_path>        build project in path (default: .)\n"
          );
}

int gadget_deploy(int argc,char **argv)
{
    int c, ret=0;
    struct mg_mgr mgr;
    struct mg_connection *nc=NULL;
    char   *project_path=NULL;
    char   *payload_path=NULL;
    gadget_project_t *project=0;
    subprocess_t *p;
    char   *tmpstr=0;

    while (1)
    {
        static struct option long_options[] =
        {
            {"verbose", no_argument,       0, 'v'},
            {"help",    no_argument,       0, 'h'},
            {0, 0, 0, 0}
        };

        int option_index = 0;

        c = getopt_long (argc, argv, "hv",
                long_options, &option_index);

        /* Detect the end of the options. */
        if (c == -1)
            break;

        switch (c)
        {
            case 'v':
                _VERBOSE=1;
                break;

            case 'h':
                gadget_deploy_help();
                ret=0;
                goto _return;

            case '?':
                /* getopt_long already printed an error message. */
                break;

            default:
                abort ();
        }
    }

    if(optind == argc) {
        project_path=".";
    } else {
        project_path=argv[optind++];
    }

    if(optind < argc) {
        xprint(ERROR,"gadget build: ERROR, unknown extra arguments: ");
        while (optind < argc)
            xprint(ERROR, "%s ", argv[optind++]);
        putchar ('\n');
        ret = -1;
        goto _return;
    }

    if(!xis_dir("%s/.gadget",project_path)) {
        xprint(ERROR,"gadget build: ERROR: not a gadget project: '%s'\n",project_path);
        ret=1;
        goto _return;
    }

    if(!(project=gadget_project_deserialize("%s/.gadget/config",project_path))) {
        xprint(ERROR,"gadget deploy: ERROR: cannot read project file: '%s/.gadget/config'\n",project_path);
        ret=1;
        goto _return;
    }

    // rebuild container image, just in case...
    gadget_project_build(project);

    char *tmpdir=0;
    asprintf(&tmpdir,"/tmp/gadget_build_XXXXXX");
    xprint(VERBOSE,"mkdtemp('%s'): ",tmpdir);
    if(!mkdtemp(tmpdir)) {
        xprint(ERROR,"gadget deploy: ERROR: cannot create directory '%s'\n",tmpdir);
        ret=1;
        goto _return;
    }
    xprint(VERBOSE,"%s\n",tmpdir);
    if(asprintf(&payload_path,"%s/%s_%s.tar",tmpdir,project->name,project->id)<0)
    {
        xprint(ERROR,"gadget deploy: ERROR: asprintf returned negative value\n");
        goto _return;
    }

    xprint(VERBOSE,"creating %s\n",payload_path);
    p=subprocess_run_gw("docker","save",project->container_image_name,"-o",payload_path,0);
    if(p->exit) {
        xprint(ERROR,"gadget build: ERROR: start subprocess '%s' failed:\n\n",p->cmdline);
        xprint(ERROR,"%s\n%s%s\n", p->cmdline, p->out, p->err);
        goto _return;
    }
    xprint(VERBOSE,"%s\n%s%s\n", p->cmdline, p->out, p->err);
    subprocess_free(p); p=0;

    if(!xis_file(payload_path)) {
        xprint(ERROR,"gadget deploy: ERROR: cannot read: '%s'\n",payload_path);
        ret=1;
        goto _return;
    }

    xprint(NORMAL,"deploying %s...", project->name);
    do_rpc(ENDPOINT_VERSION,project);            xprint(NORMAL,".");
    do_rpc(ENDPOINT_APPLICATION_STOP,project);   xprint(NORMAL,".");
    do_rpc(ENDPOINT_APPLICATION_DELETE,project); xprint(NORMAL,".");
    do_rpc(ENDPOINT_APPLICATION_PURGE,project);  xprint(NORMAL,".");

    mg_mgr_init(&mgr, NULL);
    xprint(VERBOSE,"sending %s...\n",payload_path);
    tmpstr=build_url(ENDPOINT_APPLICATION_ADD,project);
    nc = mg_postfile_http(&mgr, ev_handler, tmpstr, payload_path);
    mg_set_protocol_http_websocket(nc);
    xprint(VERBOSE,"requesting %s\n", tmpstr);
    while (s_exit_flag == 0) { mg_mgr_poll(&mgr, 1000); xprint(NORMAL,".", project->name); }

    do_rpc(ENDPOINT_APPLICATION_CREATE,project); xprint(NORMAL,".");
    do_rpc(ENDPOINT_APPLICATION_START,project);  xprint(NORMAL,"done!");

    mg_mgr_free(&mgr);

_return:
    if(tmpstr) free(tmpstr);
    if(payload_path) free(payload_path);
    if(project) gadget_project_destruct(project);

    return ret;
}
