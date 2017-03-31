#include "utils.h"
#include "config.h"
#include <string.h>
int main(int argc, char** argv) {
	if(argc < 2) return 1;

	if(strcmp(argv[1], "xmkpath") == 0) {
		if(xmkpath("testproject/.gadget", 0775)) {
			fprintf( stderr, "failure\n");
			return 1;
		}
	}
	else if(strcmp(argv[1], "subprocess_run") == 0) {
        subprocess_t *p;
        p=subprocess_run("echo","hello","world",0);
        subprocess_grab_output(p);
        if(p->exit) {
            xprint(ERROR,"subprocess(): running '%s' failed.\n",p->cmdline);
            subprocess_free(p);
            return 1;
        }
        char *expect="hello world\n";
        if(strcmp(p->out,expect)) {
            xprint(ERROR,"subprocess(): unexpected output '%s' from running '%s' (expected: '%s')\n",p->out,p->cmdline,expect);
            subprocess_free(p);
            return 1;
        }
        subprocess_free(p);
    }
	return 0;
}
