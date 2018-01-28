#ifndef __SUDOHULK_H__
#define __SUDOHULK_H__

#ifdef __x86_64__
	#define syscall_reg(x) ((x).orig_rax)
	#define syscall_first_parameter(x) ((x).rdi)
	#define syscall_second_parameter(x) ((x).rsi)
#elif __i386__
	#define syscall_reg(x) ((x).orig_eax)
	#define syscall_first_parameter(x) ((x).ebx)
	#define syscall_second_parameter(x) ((x).ecx)
#endif

#endif
