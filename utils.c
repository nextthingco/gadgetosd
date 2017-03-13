#define _GNU_SOURCE

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <libgen.h>
#include <errno.h>
#include <sys/stat.h>
#include <uuid/uuid.h>

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

int xis_dir(const char *path) {
   struct stat statbuf;
   if (stat(path, &statbuf) != 0)
       return 0;
   return S_ISDIR(statbuf.st_mode);
}


int xmkpath(char *dir, mode_t mode)
{
    int ret=0;
    char *cpy=0;

    if(!dir) { errno = EINVAL; ret=1; goto _return; }
    if(xis_dir(dir)) { ret=0; goto _return; }

    xmkpath(dirname((cpy=strdup(dir))), mode);

_return:
    if(cpy) free(cpy);
    return mkdir(dir, mode);
}

int xmkdir(mode_t mode, const char *format, ...)
{
    char *tmpstr=0;
    int ret=0;
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
    int r,w,total=0;

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

int xcp(char *source,char *destination) {
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
    if (in == NULL) return 1;
    
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
    *output_buf=ptr;

    return ret;
}
