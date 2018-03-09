#ifndef __SUDOHULK_H__
#define __SUDOHULK_H__

#ifdef __x86_64__
	#define syscall_reg(x) ((x).orig_rax)
	#define syscall_first_parameter(x) ((x).rdi)
	#define syscall_second_parameter(x) ((x).rsi)
	#define USER_STRUCT user_regs_struct
#elif __i386__
	#define syscall_reg(x) ((x).orig_eax)
	#define syscall_first_parameter(x) ((x).ebx)
	#define syscall_second_parameter(x) ((x).ecx)
	#define USER_STRUCT user_regs_struct
#elif __arm__
	#define syscall_reg(x) ((x).regs.ARM_r7)
	#define syscall_first_parameter(x) ((x).regs.ARM_r0)
	#define syscall_second_parameter(x) ((x).regs.ARM_r1)
	#define USER_STRUCT user
#endif



#endif
