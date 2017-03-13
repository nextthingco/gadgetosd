#ifndef _UTILS_H_
#define _UTILS_H_

extern char *xuuid_generate();
extern char* xsprintf(const char *format, ...);
extern int xmkpath(char *dir, mode_t mode);
extern int xmkdir(mode_t mode, const char *format, ...);
extern int xis_dir(const char *path);
extern int xfcp(FILE *src, FILE *dst);
extern int xcp(const char *source, const char *destination);
extern int xfrun(const char *cmd, FILE *out);
extern long xfsize(FILE *fp);
extern char *xfread(FILE* in);

#endif //_UTILS_H_
