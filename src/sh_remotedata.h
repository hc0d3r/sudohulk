#ifndef __SH_REMOTEDATA__
#define __SH_REMOTEDATA__

#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/user.h>

#include <linux/ptrace.h>

#include <stdio.h>
#include <errno.h>


#include "sh_common.h"
#include "sh_string.h"

char *get_remote_string(pid_t pid, long addr);
void write_remote_data(pid_t pid, const void *data, size_t len, long addr);
char **get_remote_string_array(pid_t pid, long addr);
void write_remote_double_char(pid_t pid, long remote_addr, char **local_array);

#endif
