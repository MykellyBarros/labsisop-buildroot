#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include "sleepProcessInfo.h"

asmlinkage long sys_listSleepProcesses(char __user *ubuf, size_t usize)
{
    struct task_struct *task;
    char *kbuf;
    size_t used = 0;
    int ret;

    /* Validação do tamanho de entrada: não vazio, não excessivo */
    if (usize == 0 || usize > (1 << 20))
        return -EINVAL;

    /* Aloca dinamicamente o buffer de kernel */
    kbuf = kmalloc(usize, GFP_KERNEL);
    if (!kbuf)
        return -ENOMEM;

    /* Percorre todos os processos e monta a lista de sleepers */
    for_each_process(task) {
        if (task->state == TASK_INTERRUPTIBLE ||
            task->state == TASK_UNINTERRUPTIBLE) {

            size_t left = usize - used;
            int written;

            if (left <= 0)
                break;

            written = snprintf(kbuf + used, left,
                               "PID: %d | Name: %s | State: %ld\n",
                               task_pid_nr(task), task->comm, task->state);
            if (written < 0 || written >= left)
                break;

            used += written;
        }
    }

    /* Copia para o espaço de usuário */
    if (copy_to_user(ubuf, kbuf, used)) {
        kfree(kbuf);
        return -EFAULT;
    }

    kfree(kbuf);
    return used;
}