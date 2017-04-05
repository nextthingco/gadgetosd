#ifndef _GADGET_PROJECT_H
#define _GADGET_PROJECT_H

typedef struct gadget_project_t {
    char *name;
    char *id;
    char *path;
    char *container_name;
    char *container_image_name;
} gadget_project_t;

// create a new gadget_project_t
gadget_project_t* gadget_project_create( char *name );

// free memory
void gadget_project_destruct(gadget_project_t *p);

// serialize into ini file
int gadget_project_serialize( char* filename, gadget_project_t *p );

// de-serialize from ini file
gadget_project_t* gadget_project_deserialize( char* filenameformat, ... );

// build project
int gadget_project_build(gadget_project_t *self);

#endif
