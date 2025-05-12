#ifndef _MESSAGE_SYSCALL_H
#define _MESSAGE_SYSCALL_H

#include <linux/types.h>
#include <linux/uaccess.h>

asmlinkage long sys_printUserMessage(const char __user *msg, size_t len);

#endif /* _MESSAGE_SYSCALL_H */