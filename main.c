#include <stdio.h>
#include <string.h>

extern int gadgetosd(int argc, char **argv);
extern int gadget(int argc, char **argv);

int main(int argc, char **argv)
{
    char* n = (n=strrchr(argv[0],'/')) ? n+1 : argv[0]; 

    if( strcmp( n,"gadget")==0) {
        return gadget(argc,argv);
    } else {
        return gadgetosd(argc,argv);
    }
}
