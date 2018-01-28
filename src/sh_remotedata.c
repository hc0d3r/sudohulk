#include "sh_remotedata.h"

void write_remote_data(pid_t pid, const void *data, size_t len, long addr){
	size_t i;
	long old_data, new_data = 0L;

	for(i=0; (i+__wordsize)<len; i+=__wordsize){
		ptrace(PTRACE_POKEDATA, pid, addr+i, *(long *)(data+i));
	}

	len -= i;

	if(len == __wordsize){
		new_data = *(long *)(data+i);
	} else {
		memcpy(&new_data, data+i, len);
		old_data = ptrace(PTRACE_PEEKDATA, pid, addr+i, 0L);
		old_data &= (unsigned long)-1 << (8*len);
		new_data |= old_data;
	}

	ptrace(PTRACE_POKEDATA, pid, addr+i, new_data);

}

char **get_remote_string_array(pid_t pid, long addr){
	char **ret = NULL;
	int count = 0;
	long read_bytes;
	int i = 0;

	char *args[1024];

	while(1){
		read_bytes = ptrace(PTRACE_PEEKDATA, pid, (addr+i), 0L);
		if(errno || !read_bytes)
			break;

		if( (args[count] = get_remote_string(pid, read_bytes)) == NULL ){
			printf("get_remote_string error\n");
			break;
		}

		count++;
		i += __wordsize;
	}

	if(!count)
		return NULL;

	ret = malloc((count+1) * sizeof(char *));
	for(i=0; i<count; i++)
		ret[i] = args[i];

	ret[i] = NULL;

	return ret;
}

char *get_remote_string(pid_t pid, long addr){
	int i, j, end = 0;
	long read_bytes;
	dynamic_str_t string = {.size = 0, .ptr = NULL};

	for(i=0; !end; i+=__wordsize){
		read_bytes = ptrace(PTRACE_PEEKDATA, pid, (addr+i), 0L);
		if(errno)
			break;

		for(j=0; j<(int)__wordsize; j++){
			char c = ((read_bytes >> (8*j)) & 0xff);
			if(!c){
				dynamic_write(&string, &read_bytes, (j+1));
				end = 1;
				break;
			}
		}

		if(j == __wordsize){
			dynamic_write(&string, &read_bytes, __wordsize);
		}
	}

	return string.ptr;
}

void write_remote_double_char(pid_t pid, long remote_addr, char **local_array){
	int i, addr_size, argc;

	argc = args_count(local_array);
	addr_size = (argc+1)*__wordsize; // +1 for 0x0000000000000000

	long remote_addr_offset;
	int next_size = 0;
	remote_addr_offset = remote_addr+addr_size;


	for(i=0; i<argc; i++){
		remote_addr_offset += next_size;
		write_remote_data(pid, &remote_addr_offset, __wordsize, remote_addr + (i*__wordsize));

		next_size = strlen(local_array[i])+1; // +1 because we need to write the null byte
		write_remote_data(pid, local_array[i], next_size, remote_addr_offset);
	}

	remote_addr_offset = 0L;
	write_remote_data(pid, &remote_addr_offset, __wordsize, remote_addr + addr_size - __wordsize);
}
