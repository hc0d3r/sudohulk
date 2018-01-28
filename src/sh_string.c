#include "sh_string.h"

void dynamic_write(dynamic_str_t *out, const void *bytes, size_t len){
	size_t aux = 0;

	if(!out->size){
		out->size = len;
		out->ptr = malloc(len);
	} else {
		aux = out->size;
		out->size += len;
		out->ptr = realloc(out->ptr, out->size);
	}

	memcpy(out->ptr+aux, bytes, len);
}
