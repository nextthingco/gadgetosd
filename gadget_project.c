/*
 * ex: softtabstop=4 shiftwidth=4 tabstop=4 expandtab
 *
 * Copyright (c) 2017 Next Thing Co
 * All rights reserved
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>

#include "gadget_project.h"
#include "utils.h"
#include "config.h"
#include "ini.h"
#include "docker.h"


//helper function
char *gadget_project_container_image_name( gadget_project_t* p )
{
    if(!p) return 0;
    if(p->container_image_name) return p->container_image_name;
    if(!p->name || !p->id) return 0;

    if(asprintf(&p->container_image_name,"%s_%s",p->name,p->id)<0)
        return 0;

    return p->container_image_name;
}

//helper function
char *gadget_project_container_name( gadget_project_t* p )
{
    if(!p) return 0;
    if(p->container_name) return p->container_name;
    if(!p->name || !p->id) return 0;

    if(asprintf(&p->container_name,"%s_%s_c",p->name,p->id)<0)
        return 0;

    return p->container_name;
}

// create a new gadget_project_t
gadget_project_t* gadget_project_create( char *name )
{
    gadget_project_t *p = 0;

    if(!(p=(gadget_project_t*)malloc(sizeof(gadget_project_t)))) goto _error;
    memset(p,0,sizeof(gadget_project_t));

    if(!(p->name=strdup(name))) goto _error;
    if(!(p->id=xuuid_generate())) goto _error;

    if(!gadget_project_container_name(p)) goto _error;
    if(!gadget_project_container_image_name(p)) goto _error;

    return p;

_error:
    if(p) gadget_project_destruct(p);
    return 0;
}

void gadget_project_destruct(gadget_project_t *p)
{
    if(p) {
        if(p->name)                 free(p->name);
        if(p->id)                   free(p->id);
        if(p->container_name)       free(p->container_name);
        if(p->container_image_name) free(p->container_image_name);
        free(p);
    }
}

int gadget_project_serialize( char* filename, gadget_project_t* p )
{
    FILE *f =  0;

    if(!p) return -1;

    if(!(f=fopen(filename,"w"))) return -1;
    fprintf(f,
            "[project]\n"
            "name = %s\n"
            "id = %s\n",
            p->name,
            p->id
    );
    fclose(f);

    return 0;
}

static int gadget_project_ini_handler(void* user,
                                      const char* section,
                                      const char* name,
                                      const char* value)
{
    gadget_project_t* p = (gadget_project_t*)user;

    #define MATCH(s, n) strcasecmp(section, s) == 0 && strcasecmp(name, n) == 0
    //TODO: in windows to stricmp()

    if (MATCH("project", "name")) {
        p->name = strdup(value);
    } else if (MATCH("project", "id")) {
        p->id = strdup(value);
    } else {
        return 0;  /* unknown section/name, error */
    }

    return 1;
}

gadget_project_t* gadget_project_deserialize( char* filenameformat, ... )
{
    char *filename = 0;
    gadget_project_t *p = 0;
    va_list args;

    if(!(p=(gadget_project_t*)malloc(sizeof(gadget_project_t)))) goto _error;
    memset(p,0,sizeof(gadget_project_t));

    va_start(args, filenameformat);
    if(vasprintf(&filename, filenameformat, args)<0) goto _error;
    va_end(args);

    if(ini_parse(filename, gadget_project_ini_handler, p) < 0) {
        xprint(ERROR,"gadget_project_deserialize(): can't load '%s'\n",filename);
        goto _error;
    }

    p->path=dirname(dirname(strdup(filename)));

    if(!(p->name) || !strlen(p->name) || !(p->id) || !strlen(p->id)) {
        xprint(ERROR,"gadget_project_deserialize(): incompatible project file '%s'\n",filename);
        goto _error;
    } else {
        xprint(VERBOSE,"gadget_project_deserialize(): project loaded from '%s': name=%s, id=%s\n",
            filename, p->name, p->id );
    }

    if(!gadget_project_container_name(p)) goto _error;
    if(!gadget_project_container_image_name(p)) goto _error;

    goto _return;
_error:
    if(p) gadget_project_destruct(p);
    p=0;

_return:
    if(filename) free(filename);
    return p;
}

int gadget_project_build( gadget_project_t *self )
{
    int ret=0;
    char *tmpstr=0;
    docker_t *docker=0;

    docker=docker_initialize();

    //TODO: fix this up. Right now all templates (hardcoded in here) are built all the time...
    asprintf(&tmpstr,"%s/alpine",TEMPLATE_PREFIX);
    xprint(VERBOSE,"building template from '%s'\n",tmpstr);
    if(docker_build(0,tmpstr,"gadget_template_alpine")) {
        xprint(ERROR,"gadget build: ERROR: cannot build template: '%s'\n",tmpstr);
        ret=1;
        goto _return;
    }

    if(docker_build(0,self->path,self->container_image_name)) {
        xprint(ERROR,"gadget build: ERROR: cannot build container image: '%s'\n",tmpstr);
        ret=1;
        goto _return;
    }

_return:
    if(docker) docker_free(docker);

    return ret;
}
