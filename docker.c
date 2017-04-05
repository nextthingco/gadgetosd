#include <string.h>

#define _DOCKER_C_
#include "docker.h"

docker_t* docker_initialize()
{
    //TODO
    return 0;
}

void docker_free(docker_t* self)
{
    //TODO
}

int docker_build( docker_t* self, const char *path, const char* name)
{
    int ret=1;
    subprocess_t *p=0;

    if( !path || !name || !strlen(path) || !strlen(name) ) goto _return;

    p=subprocess_run_gw("docker","build","-t",name,path,0);
    if(p->exit) {
        xprint(ERROR,"gadget build: ERROR: start subprocess '%s' failed:\n\n",p->cmdline);
        xprint(ERROR,"%s\n%s%s\n", p->cmdline, p->out, p->err);
        ret=p->exit;
        goto _return;
    }
    xprint(VERBOSE,"%s\n%s%s\n", p->cmdline, p->out, p->err);
    subprocess_free(p); p=0;
    ret=0;

_return:
    if(p) subprocess_free(p);
    return ret;
}
