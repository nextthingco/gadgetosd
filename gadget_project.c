/*
 * ex: softtabstop=4 shiftwidth=4 tabstop=4 expandtab
 *
 * Copyright (c) 2017 Next Thing Co
 * All rights reserved
 */

#include <yaml.h>
#include "gadget_project.h"
#include "utils.h"
#include "ini.h"

// create a new gadget_project_t
gadget_project_t* gadget_project_create( char *name )
{
    gadget_project_t *p = 0;

    if(!(p=malloc(sizeof(gadget_project_t)))) goto _error;
    if(!(p->name=strdup(name))) goto _error;
    if(!(p->id=xuuid_generate())) goto _error;

    return p;

_error:
    if(p) gadget_project_destruct(p);
    return 0;
}

void gadget_project_destruct(gadget_project_t *p)
{
    if(p) {
        if(p->name) free(p->name);
        if(p->id)   free(p->id);
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

    #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0

    if (MATCH("project", "name")) {
        p->name = strdup(value);
    } else if (MATCH("project", "id")) {
        p->id = strdup(value);
    } else {
        return 0;  /* unknown section/name, error */
    }

    return 1;
}

gadget_project_t* gadget_project_deserialize( char* filename )
{
    gadget_project_t *p = 0;

    if(!(p=malloc(sizeof(gadget_project_t)))) {
        return 0;
    }

    if(ini_parse(filename, gadget_project_ini_handler, p) < 0) {
        fprintf(stderr,"gadget_project_deserialize(): can't load '%s'\n",filename);
        return 0;
    }

    if(!(p->name) || !strlen(p->name) || !(p->id) || !strlen(p->id)) {
        fprintf(stderr,"gadget_project_deserialize(): incompatible project file '%s'\n",filename);
        gadget_project_destruct(p);
        return 0;
    } else {
        fprintf(stderr,"gadget_project_deserialize(): project loaded from '%s': name=%s, id=%s\n",
            filename, p->name, p->id );
    }   

    return p;
}
