#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/syscalls.h>
#include "processInfo.h"

asmlinkage long sys_listSleepProcesses(char __user *buf, int size) {
    struct task_struct *proces;
    unsigned char kbuf[8192];  // Aumente o buffer para 8192 bytes
    int bufsz = 0;
    int ret;

    /* Loop através de todos os processos */
    for_each_process(proces) {
        // Verifica se o processo está em estado de sleep
        if (proces->state == TASK_INTERRUPTIBLE || proces->state == TASK_UNINTERRUPTIBLE) {
            /* Adiciona as informações do processo no buffer */
            int space_left = sizeof(kbuf) - bufsz;
            if (space_left <= 0) {
                return -ENOMEM;  // Sem espaço suficiente no buffer
            }

            bufsz += snprintf(kbuf + bufsz, space_left, "Process: %s\n PID_Number: %ld\n State: %ld\n\n", 
                               proces->comm, 
                               (long)task_pid_nr(proces), 
                               (long)proces->state);
        }
    }

    /* Verifique se o buffer do usuário é grande o suficiente */
    if (size < bufsz) {
        return -ENOMEM;  // O buffer do usuário é muito pequeno
    }

    /* Copie as informações para o buffer do usuário */
    ret = copy_to_user(buf, kbuf, bufsz);
    if (ret != 0) {
        return -EFAULT;  // Se falhar ao copiar para o espaço de usuário
    }

    return bufsz;  // Retorna o número de bytes copiados
}
