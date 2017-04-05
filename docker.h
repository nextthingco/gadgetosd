#ifndef _DOCKER_H_
#define _DOCKER_H_

#include "utils.h"

#if defined(_UTILS_C)
#define EXTERN
#else
#define EXTERN extern
#endif

typedef struct dockerfile_t {
    char *name;
    FILE *fp;
} dockerfile_t;

EXTERN int dockerfile_get(dockerfile_t f, const char* cmd, char ***dest);


typedef struct docker_t {
    int dummy;
} docker_t;

EXTERN docker_t* docker_initialize();
EXTERN void docker_free( docker_t* self );
EXTERN int docker_build( docker_t* self, const char *path, const char* name);

#endif
