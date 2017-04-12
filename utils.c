/*
 * ex: softtabstop=4 shiftwidth=4 tabstop=4 expandtab
 *
 * Copyright (c) 2017 Next Thing Co
 * All rights reserved
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <libgen.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <uuid/uuid.h>

#define _UTILS_C
#include "utils.h"
#include "config.h"

#if _WIN32
#  include <stdlib.h>
#elif __APPLE__
#  include <mach-o/dyld.h>
#  include <sys/syslimits.h>
#else // asume we are in Linux
#  include <unistd.h>
#  include <limits.h>
#endif

char *get_exe_path()
{
    char *exe_path=0;
#if _WIN32
    if (_get_pgmptr(&exe_path) != 0) exe_path = 0;
#elif __APPLE__
    exe_path=malloc(sizeof(char)*PATH_MAX);
    uint32_t len = PATH_MAX;
    if (_NSGetExecutablePath(exe_path, &len) != 0) {
        exe_path[0] = '\0'; // buffer too small (!)
    } else {
        // resolve symlinks, ., .. if possible
        char *canonicalPath = realpath(exe_path, NULL);
        if (canonicalPath != NULL) {
            strncpy(exe_path,canonicalPath,len);
            free(canonicalPath);
        }
    }
#else // assume we are in linux
    exe_path=malloc(sizeof(char)*PATH_MAX);
    ssize_t len = readlink("/proc/self/exe", exe_path,PATH_MAX);
    if (len == -1 || len == sizeof(exe_path))
        len = 0;
    exe_path[len] = '\0';
#endif
    return exe_path;
}

char* xsstrip(char *s)
{
    int i = strlen(s)-1;
    while( i>=0 && ( s[i]=='\n'
                  || s[i]=='\r'
                  || s[i]=='\t'
                  || s[i]==' ' ) ) {
        s[i--]=0;
    }
    return s;
}

char *xuuid_generate()
{
  uuid_t uuid;

  char *s_uuid=malloc(sizeof(char)*33);

  if(!s_uuid) return NULL;

  uuid_generate(uuid);
  for(int i=0;i<16;i++) { snprintf(s_uuid+(i*2),3,"%02x",uuid[i]); }

  return s_uuid;
}

char* xsprintf(const char *format, ...)
{
    char *buf=0;
    int ret=0;

    va_list args;
    va_start(args, format);
    ret=vasprintf(&buf, format, args);
    va_end(args);

    if(ret<0) {
        if(buf) free(buf);
        buf=0;
    }

    return buf;
}

int xis_file(const char *format, ...) {
    struct stat statbuf;
    char *tmpstr=0;
    int ret=0;

    va_list args;
    va_start(args, format);
    ret=vasprintf(&tmpstr, format, args);
    va_end(args);
    if(ret<0) {
        fputs("xis_dir(): ERROR: vasprintf() returned negative value",stderr);
        ret=0;
        goto _return;
    }

    if (stat(tmpstr, &statbuf) != 0) { ret=0; goto _return; }
    ret=S_ISREG(statbuf.st_mode);

_return:
    if(tmpstr) free(tmpstr);
    return ret;
}

int xis_dir(const char *format, ...) {
    struct stat statbuf;
    char *tmpstr=0;
    int ret=0;

    va_list args;
    va_start(args, format);
    ret=vasprintf(&tmpstr, format, args);
    va_end(args);
    if(ret<0) {
        fputs("xis_dir(): ERROR: vasprintf() returned negative value",stderr);
        ret=0;
        goto _return;
    }

    if (stat(tmpstr, &statbuf) != 0) { ret=0; goto _return; }
    ret=S_ISDIR(statbuf.st_mode);

_return:
    if(tmpstr) free(tmpstr);
    return ret;
}

char *xdirname(const char *path)
{
    char *tmp = strdup(path);
    char *r = strdup(dirname(tmp));
    free(tmp);
    return r;
}


int xmkpath(char *dir, mode_t mode)
{
    int ret = 0;
    if(!dir) {
        errno = EINVAL;
        return 1;
    }

    if(xis_dir(dir)) { return 0; }

    char *parent = xdirname(dir);

    // if parent isn't .
    if(strcmp(parent, ".")) {
        // attempt to create parent
        if(xmkpath(parent, mode)) {
            ret = 1;
            goto _return;
        }
    }

    ret = mkdir(dir, mode);
_return:
    free(parent);
    return ret;

}

int xmkdir(mode_t mode, const char *format, ...)
{
    char *tmpstr=0;
    int ret = 0;
    va_list args;

    va_start(args, format);
    ret=vasprintf(&tmpstr, format, args);
    va_end(args);
    if(ret<0) { goto _return; }

    ret=xmkpath(tmpstr,mode);

_return:
    if(tmpstr) free(tmpstr);
    return ret;
}

int xfcp(FILE *src, FILE *dst)
{
    const int buf_size=4096;
    static char buf[4096];
    int r = 0,w = 0, total = 0;

    while(!feof(src)) {
        r=fread(buf,sizeof(char),buf_size,src);
        if( r < 0 ) {
            fprintf(stderr,"ERROR: while reading\n");
            return r;
        }

        w=fwrite(buf,sizeof(char),r,dst);
        if( w < 0 ) {
            fprintf(stderr,"ERROR: while writing\n");
            return w;
        }
        total+=w;
    }

    return w;
}

int xcp(const char *source,const char *destination) {
    FILE *src=0,*dst=0;
    int ret=0;
    char path_sep='/';
    char *p=0,*q=0;
    int p_size=0;

    if( !(src = fopen(source,"r")) ) {
        fprintf(stderr,"ERROR: cannot open '%s'\n",source);
        ret=-1;
        goto _return;
    }

    if( xis_dir( destination ) )
    {
        q=strrchr(source,path_sep)+1;
        p_size= strlen(destination)+strlen(q)+2;
        p=malloc(sizeof(char)*p_size);
        if(!p) { ret=-1; goto _return; }
        snprintf(p,p_size,"%s%c%s",destination,path_sep,q);
        destination=p;
    }

    if( !(dst = fopen(destination,"w")) ) {
        fprintf(stderr,"ERROR: cannot write to '%s'\n",destination);
        goto _return;
    }

    ret=xfcp(src,dst);

_return:
    if(p) free(p);
    if(src) fclose(src);
    if(dst) fclose(dst);
    return ret;
}

long xfsize(FILE *fp)
{
    long size=0;
    long pos=ftell(fp);

    fseek(fp, 0, SEEK_END);
    size=ftell(fp);
    fseek(fp, pos, SEEK_SET);
    return(size);
}

char *xfread(FILE* in)
{
    char *buf=0,*ptr=0;
    int read=0;
    long pos=0;
    int size=0;

    pos=ftell(in);
    fseek(in, 0, SEEK_END);
    size=ftell(in)-pos;
    fseek(in, pos, SEEK_SET);

    buf = malloc(sizeof(char)*(size+1));
    if(!buf) {
        fprintf(stderr,"ERROR: cannot allocate memory\n");
        return NULL;
    }

    ptr=buf;
    while(!feof(in) ) {
        read=fread(ptr,sizeof(char),4096,in);
        if(read<0) {
            fprintf(stderr,"ERROR: while reading\n");
        }
        ptr+=read;
    }
    buf[size]=0;

    return buf;
}

char *xreadfile( const char *path )
{
    FILE *in=NULL;
    char *contents=NULL;

    in=fopen(path,"r");
    if(!in) {
        fprintf(stderr,"ERROR: cannot read '%s'\n",path);
        return NULL;
    }

    contents = xfread(in);
    fclose(in);

    return contents;
}

int xfrun(const char *cmd, FILE *out)
{
    FILE *in=NULL;
    int ret=0;

    if(!out) return 1;
    in = popen(cmd, "r");
    if (in == NULL) goto _return;

    if((ret=xfcp(in,out))<0) {
        pclose(in);
        return ret;
    }

_return:
    if(in) ret=pclose(in);
    return ret;
}

int xrun(const char *cmd, char **output_buf)
{
    FILE *fp=NULL;
    int ret=0;
    char *ptr=NULL;

    if(!(fp=tmpfile())) {
        fprintf(stderr,"ERROR: cannot create temp file.");
        return -1;
    }

    ret=xfrun(cmd,fp);
    fseek(fp,0,SEEK_SET);
    ptr=xfread(fp);
    fclose(fp);
    *output_buf=ptr;

    return ret;
}

// helper function
char** vbuild_argv(char *cmd, va_list varargs )
{
    int     argc=0;
    char    **argv=0;

    if(!(argv=malloc(sizeof(char*)))) goto _error;
    argv[argc]=strdup(cmd);
    argc++;

    while(1) {
        argv=realloc(argv,sizeof(char*)*(argc+1));

        char *p = va_arg(varargs, char*);
        //note: the conversion to (int) is necessary
        //      to enable vbuild_argv(cmd,arg1,arg2,0)
        //      and not only vbuild_argv(cmd,arg1,arg2,(char*)0).
        //      seems to be some variadic madness that
        //      has been observed on mac os using clang
        if((int)p == 0) {
            argv[argc] = (char*) 0;
            break;
        } else {
            argv[argc] = strdup(p);
            //xprint(VERBOSE,"vbuild_argv(): argv[%d]=%s\n",argc,argv[argc]);
        }
        argc++;
    }

    return argv;

_error:
    xprint(VERBOSE,"ERROR!\n");
    return 0;
}

int xpopen(int *pipes, char* cmd, char **argv)
{
    int     in[2];
    int     out[2];
    int     err[2];
    pid_t   pid;

    // create pipes
    if(pipe(in))  goto error_in;
    if(pipe(out)) goto error_out;
    if(pipe(err)) goto error_err;

    pid = fork();
    switch (pid) {
     case -1: // error
         goto error_fork;
     case 0: // child process
         close(in[1]);
         close(out[0]);
         close(err[0]);

         dup2(in[0], 0);   // use in[0] as stdin
         dup2(out[1], 1);  // use out[1] as stdout
         dup2(err[1], 2);  // use err[1] as stderr

         execvp(cmd,argv);

         return -1;        // shall never be executed
     default: // parent process
         close(in[0]);  // not used in parent
         close(out[1]); // not used in parent
         close(err[1]); // not used in parent

         pipes[0] = in[1];  // write to child stdin
         pipes[1] = out[0]; // read child stdout
         pipes[2] = err[0]; // read child stderr
         return pid;
    }

error_fork:
    close(err[0]);
    close(err[1]);
error_err:
    close(out[0]);
    close(out[1]);
error_out:
    close(in[0]);
    close(in[1]);
error_in:
    return -1;
}

int xpopenl(int *pipes, char *cmd, ...)
{
    va_list varargs;
    char **argv;

    va_start(varargs, cmd);
    argv=vbuild_argv(cmd, varargs);
    va_end(varargs);

    return xpopen(pipes,cmd,argv);
}

int xpclose(const pid_t pid, int *pipes)
{
    int s;

    waitpid(pid, &s, 0);
    close(pipes[0]);
    close(pipes[1]);
    close(pipes[2]);

    return s;
}

subprocess_t *subprocess_run(char *cmd, ...)
{
    char **argv;
    va_list varargs;

    va_start(varargs,cmd);
    argv=vbuild_argv(cmd,varargs);
    va_end(varargs);

    return subprocess_runv(cmd,argv);
}

subprocess_t *subprocess_run_gw(char *cmd, ...)
{
    va_list varargs;
    char **argv;
    subprocess_t *p;

    va_start(varargs,cmd);
    argv=vbuild_argv(cmd,varargs);
    va_end(varargs);

    p=subprocess_runv(cmd,argv);
    subprocess_grab_output(p);
    
    return p;
}

subprocess_t *subprocess_runv(char *cmd, char **argv)
{
    subprocess_t *r=0;
    int len=0;
    char *p;
    int i;

    if(!(r=malloc(sizeof(subprocess_t)))) goto _error;
    memset(r,0,sizeof(subprocess_t));
    r->cmd=strdup(cmd);
    r->argv=argv;
    if(!r->argv) goto _error;

    for(r->argc=0; r->argv[r->argc]!=0; r->argc++) {
        len += strlen(r->argv[r->argc]);
    }

    if(!(r->cmdline=malloc(sizeof(char)*(len+1+r->argc)))) goto _error;
    for(i=0, p=r->cmdline; i<r->argc; i++) {
        if(!(p=stpcpy(p,r->argv[i]))) goto _error;
        *p=' ';
        p++;
    }
    *(--p)=0;

    r->exit    = -314151;
    r->status  = -314151;
    r->max_out = 8196;
    r->out     = 0;
    r->err     = 0;

    r->pid = xpopen(r->pipes,r->cmd,r->argv);
    return r;

_error:
    if(r) subprocess_free(r);
    return 0;
}

int subprocess_wait(subprocess_t *p)
{
    if(waitpid(p->pid, &p->status, 0)!=p->pid) {
        xprint(VERBOSE,"ERROR: waitpid returned negative value\n");
        return errno;
    }
    if(WIFEXITED(p->status)) {
        p->exit = WEXITSTATUS(p->status);
    } else {
        xprint(VERBOSE,"ERROR: child quit abnormally\n");
    }

    return p->exit;
}

int subprocess_grab_output(subprocess_t *p)
{
    FILE *f=0,*g=0;
    int  i=0,j=0;

    f = fdopen(p->pipes[1],"r");
    g = fdopen(p->pipes[2],"r");

    p->out = malloc(sizeof(char)*(p->max_out+1));
    p->err = malloc(sizeof(char)*(p->max_out+1));

    i=j=0;
    while( !(feof(f) && feof(g)) ) {
        int c;
        if((c=fgetc(f))!=EOF) { if(i<p->max_out) p->out[i++]=c; }
        if((c=fgetc(g))!=EOF) { if(j<p->max_out) p->err[j++]=c; }
    }
    p->out[i]=p->err[j]=0;
    fclose(f);
    fclose(g);

    subprocess_wait(p);

    return p->exit;
}

void subprocess_free(subprocess_t *p) {
    if(p) {
        if(p->cmd) free(p->cmd);
        if(p->argv) {
            for(int i=0; i<p->argc; i++) {
                if(p->argv[i]) free(p->argv[i]);
            }
        }
        if(p->cmdline) free(p->cmdline);
        if(p->out) free(p->out);
        if(p->err) free(p->err);
    }
}

int xprint(int type,const char *format, ...)
{
    int ret=0;
    va_list args;

    if( type!=VERBOSE || _VERBOSE==1) {
        va_start(args, format);
        ret=vfprintf(stderr, format, args);
        va_end(args);
    }

    return ret;
}
