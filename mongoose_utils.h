#ifndef _MONGOOSE_UTILS
#define _MONGOOSE_UTILS

#include "gadget_project.h"
#include "mongoose.h"

extern int mgu_get_var(struct mg_connection *nc, void *p, char *name, char* dst, size_t dst_size);
extern int do_rpc(char *endpoint,gadget_project_t *project);
extern char *build_url( char* endpoint, gadget_project_t *project);

#endif
