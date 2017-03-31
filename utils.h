/*
 * ex: softtabstop=4 shiftwidth=4 tabstop=4 expandtab
 *
 * Copyright (c) 2017 Next Thing Co
 * All rights reserved
 */

#ifndef _UTILS_H_
#define _UTILS_H_

#include <sys/types.h>
#include <stdio.h>

#if defined(__linux__)
    #include <linux/limits.h>
#elif defined(__APPLE__)
    #include <limits.h>
#elif defined(_WIN32)
#endif

#if defined(_UTILS_C)
#define EXTERN
#else
#define EXTERN extern
#endif

#define ERROR   0
#define NORMAL  1
#define VERBOSE 2

//strips of trailing white space characters
EXTERN char* xsstrip(char *s);

EXTERN char *get_exe_path();
EXTERN char *xuuid_generate();
EXTERN char* xsprintf(const char *format, ...);
EXTERN int xmkpath(char *dir, mode_t mode);
EXTERN int xmkdir(mode_t mode, const char *format, ...);
EXTERN int xis_file(const char *format, ...);
EXTERN int xis_dir(const char *format, ...);
EXTERN int xfcp(FILE *src, FILE *dst);
EXTERN int xcp(const char *source, const char *destination);
EXTERN long xfsize(FILE *fp);
EXTERN char *xfread(FILE* in);
EXTERN char *xreadfile( const char *path );

//runs command and captures output in FILE *out
//NOTE: out has to be initialized accordingingly
EXTERN int xfrun(const char *cmd, FILE *out);

//runs command and captures output in output_buf
//NOTE: please free() output_buf yourself!
EXTERN int xrun(const char *cmd, char **output_buf);

EXTERN int xpopen(int *pipes, char *cmd, ...);
EXTERN int xpclose(const pid_t pid, int *pipes);
EXTERN int xexec(const char* process, ...);

EXTERN int xprint(int TYPE,const char *format, ...);

#endif //_UTILS_H_
