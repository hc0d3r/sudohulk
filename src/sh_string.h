#ifndef __SH_STRING__
#define __SH_STRING__

#include <string.h>
#include <stdlib.h>

typedef struct {
	size_t size;
	char *ptr;
} dynamic_str_t;


void dynamic_write(dynamic_str_t *out, const void *bytes, size_t len);

static inline int args_count(char **argv){
	int argc;
	for(argc=0; argv[argc]; argc++);
	return argc;
}

#endif
