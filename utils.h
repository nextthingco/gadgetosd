/*
 * ex: softtabstop=4 shiftwidth=4 tabstop=4 expandtab
 *
 * Copyright (c) 2017 Next Thing Co
 * All rights reserved
 */

#ifndef _UTILS_H_
#define _UTILS_H_

#include <sys/types.h>

#if defined(__linux__)
    #include <linux/limits.h>
#elif defined(__APPLE__)
    #include <limits.h>
#elif defined(_WIN32)
#endif

//strips of trailing white space characters
extern char* xsstrip(char *s);

extern char *xuuid_generate();
extern char* xsprintf(const char *format, ...);
extern int xmkpath(char *dir, mode_t mode);
extern int xmkdir(mode_t mode, const char *format, ...);
extern int xis_file(const char *format, ...);
extern int xis_dir(const char *format, ...);
extern int xfcp(FILE *src, FILE *dst);
extern int xcp(const char *source, const char *destination);
extern long xfsize(FILE *fp);
extern char *xfread(FILE* in);
extern char *xreadfile( const char *path );

//runs command and captures output in FILE *out
//NOTE: out has to be initialized accordingingly
extern int xfrun(const char *cmd, FILE *out);

//runs command and captures output in output_buf
//NOTE: please free() output_buf yourself!
extern int xrun(const char *cmd, char **output_buf);

#endif //_UTILS_H_
