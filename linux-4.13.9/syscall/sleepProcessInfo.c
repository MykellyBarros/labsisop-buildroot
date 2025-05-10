#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/syscalls.h>
#include "sleepProcessInfo.h"

asmlinkage long sys_listSleepProcesses(char __user *buf, int size) {
    struct task_struct *proces;
    unsigned char kbuf[1024];
    int bufsz = 0;
    int ret;
    
    /* Loop through all processes */
    for_each_process(proces) {
        if (proces->state == TASK_INTERRUPTIBLE || proces->state == TASK_UNINTERRUPTIBLE) {
            /* Add the process info to the buffer */
            bufsz += snprintf(kbuf + bufsz, sizeof(kbuf) - bufsz, "Process: %s\n PID_Number: %ld\n State: %ld\n\n", 
                               proces->comm, 
                               (long)task_pid_nr(proces), 
                               (long)proces->state);

            /* If buffer exceeds size, return error */
            if (bufsz >= size) {
                return -1;
            }
        }
    }

    /* Copy the collected information to user buffer */
    ret = copy_to_user(buf, kbuf, bufsz);

    return bufsz - ret;
}
