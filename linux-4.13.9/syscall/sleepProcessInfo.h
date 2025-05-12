#ifndef _SLEEP_PROCESS_INFO_H
#define _SLEEP_PROCESS_INFO_H

#include <linux/types.h>
#include <linux/uaccess.h>

asmlinkage long sys_listSleepProcesses(char __user *buf, size_t size);

#endif /* _SLEEP_PROCESS_INFO_H */