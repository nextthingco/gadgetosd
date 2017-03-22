#include "utils.h"
#include <string.h>
int main(int argc, char** argv) {
	if(argc < 2) return 1;

	if(strcmp(argv[1], "xmkpath") == 0) {
		if(xmkpath("testproject/.gadget", 0775)) {
			fprintf( stderr, "failure\n");
			return 1;
		}
	}
	else if(strcmp(argv[1], "xexec") == 0) {
		if(xexec("echo", "echo", "hello world", NULL)) {
			fprintf( stderr, "failure\n");
			return 1;
		}
	}
	return 0;
}
