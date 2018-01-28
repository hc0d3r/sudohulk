#ifndef __SUDOHULK_H__
#define __SUDOHULK_H__


void parser_args(int argc, char ***argv, sudohulk_options_t *opts);
int ptrace_loop(sudohulk_options_t *opts);
int cmd_change(pid_t pid, sudohulk_options_t *opts, struct user_regs_struct *regs);
int ptrace_attach(pid_t pid);
void help(void);

#endif
