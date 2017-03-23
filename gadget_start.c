/*
 * ex: softtabstop=4 shiftwidth=4 tabstop=4 expandtab
 *
 * Copyright (c) 2017 Next Thing Co
 * All rights reserved
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include "mongoose.h"

#include "config.h"
#include "utils.h"
#include "gadget_project.h"

static int verbose;

extern char* GADGETOSD_SERVER;
extern char* GADGETOSD_PORT;

/* RESTful server host and request URI */
static int s_exit_flag = 0;
    
static void ev_handler(struct mg_connection *nc, int ev, void *ev_data) {
  struct http_message *hm = (struct http_message *) ev_data;
  int connect_status;

  switch (ev) {
    case MG_EV_CONNECT:
      connect_status = *(int *) ev_data;
      if (connect_status != 0) {
        printf("Error connecting %s\n", strerror(connect_status));
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

void gadget_start_help()
{
    printf(
            "Create embedded Linux apps - easy.\n"
            "\n"
            "usage: gadget deploy [<project_path>]\n"
            "\n"
            "optional arguments:\n"
            "  -h, --help            show this help message and exit\n"
            "  --verbose             be verbose\n"
            "  <project_path>        build project in path (default: .)\n"
          );
}

int gadget_start(int argc,char **argv)
{
    int c, ret=0;
    struct mg_mgr mgr;
    struct mg_connection *nc=NULL;
    char   *project_path=NULL;
    char   *payload_path=NULL;
    gadget_project_t *project=0;
    char   *tmpstr=0;

    while (1)
    {
        static struct option long_options[] =
        {
            {"verbose", no_argument,       &verbose, 1},
            {"help",    no_argument,       0, 'h'},
            {0, 0, 0, 0}
        };

        int option_index = 0;

        c = getopt_long (argc, argv, "h",
                long_options, &option_index);

        /* Detect the end of the options. */
        if (c == -1)
            break;

        switch (c)
        {
            case 0:
                /* If this option set a flag, do nothing else now. */
                if (long_options[option_index].flag != 0)
                    break;
                printf("option %s", long_options[option_index].name);
                if (optarg)
                    printf (" with arg %s", optarg);
                printf ("\n");
                break;

            case 'h':
                gadget_start_help();
                ret=0;
                goto _return;

            case '?':
                /* getopt_long already printed an error message. */
                break;

            default:
                abort ();
        }
    }

    if (verbose)
        puts ("verbose flag is set\n");

    if(optind == argc) {
        project_path=".";
    } else {
        project_path=argv[optind++];
    }

    if(optind < argc) {
        fprintf(stderr,"gadget build: ERROR, unknown extra arguments: ");
        while (optind < argc)
            fprintf (stderr,"%s ", argv[optind++]);
        putchar ('\n');
        ret = -1;
        goto _return;
    }

    if(!xis_dir("%s/.gadget",project_path)) {
        fprintf(stderr,"gadget build: ERROR: not a gadget project: '%s'\n",project_path);
        ret=1;
        goto _return;
    }

    if(!(project=gadget_project_deserialize("%s/.gadget/config",project_path))) {
        fprintf(stderr,"gadget build: ERROR: cannot read project file: '%s/.gadget/config'\n",project_path);
        goto _return;
    }
    
    asprintf(&tmpstr,"%s?container=%s&image=%s",URL_APPLICATION_START,project->container_name,project->container_image_name);

    mg_mgr_init(&mgr, NULL);

    nc = mg_connect_http(&mgr, ev_handler, URL_VERSION, NULL, NULL);
    mg_set_protocol_http_websocket(nc);
    fprintf(stderr,"requesting %s\n", URL_VERSION );
    while (s_exit_flag == 0) { mg_mgr_poll(&mgr, 1000); }
    s_exit_flag=0; //needs to be reset here, otherwise the following fails!!

    nc = mg_connect_http(&mgr, ev_handler, tmpstr, NULL, NULL);
    mg_set_protocol_http_websocket(nc);
    fprintf(stderr,"requesting %s\n", URL_APPLICATION_START);
    while (s_exit_flag == 0) { mg_mgr_poll(&mgr, 1000); }
    s_exit_flag=0; //needs to be reset here, otherwise the following fails!!


    mg_mgr_free(&mgr);

_return:
    if(tmpstr) free(tmpstr);
    if(payload_path) free(payload_path);
    if(project) gadget_project_destruct(project);

    return 0;
}
