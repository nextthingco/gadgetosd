#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

int is_dir(const char *path) {
   struct stat statbuf;
   if (stat(path, &statbuf) != 0)
       return 0;
   return S_ISDIR(statbuf.st_mode);
}

int cp(char *source,char *destination) {
    const int buf_size=4096;
    static char buf[4096];
    FILE *src,*dst;
    int r=0,w=0,ret=0;
    char path_sep='/';
    char *p=0,*q=0;
    int p_size=0; 

    if( !(src = fopen(source,"r")) ) {
        fprintf(stderr,"ERROR: cannot open '%s'\n",source);
        ret=-1;
        goto _return;
    }

    if( is_dir( destination ) ) 
    {
        q=strrchr(source,path_sep)+1;
        p_size= strlen(destination)+strlen(q)+2;
        p=malloc(sizeof(char)*p_size);
        if(!p) { ret=-1; goto _return; }
        snprintf(p,p_size,"%s%c%s",destination,path_sep,q);
        destination=p;
    }

    if( (dst = fopen(destination,"w")) ) {
        while(!feof(src)) {
            r=fread(buf,sizeof(char),buf_size,src);
            if( r > 0 ) {
                w=fwrite(buf,sizeof(char),r,dst);
                if( w < 0 ) {
                    fprintf(stderr,"ERROR: while writing to '%s'\n",destination);
                    ret=w;
                    break;
                }
            }
            else {
                fprintf(stderr,"ERROR: while reading from '%s'\n",source);
                ret=r;
                break;
            }
        }
    } else {
        fprintf(stderr,"ERROR: cannot write to '%s'\n",destination);
    }

_return:
    if(p) free(p);
    if(src) fclose(src);
    if(dst) fclose(dst);
    return ret;
}


