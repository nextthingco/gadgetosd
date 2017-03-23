#ifndef _MONGOOSE_UTILS
#define _MONGOOSE_UTILS

extern int mgu_get_var(struct mg_connection *nc, void *p, char *name, char* dst, size_t dst_size);

#endif
