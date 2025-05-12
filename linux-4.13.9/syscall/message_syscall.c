#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include "message_syscall.h"

asmlinkage long sys_printUserMessage(const char __user *umsg, size_t len)
{
    char *kmsg;
    long ret;

    /* Valida tamanho: não vazio, até 1 página */
    if (len == 0 || len > PAGE_SIZE)
        return -EINVAL;

    /* Aloca buffer no kernel */
    kmsg = kmalloc(len + 1, GFP_KERNEL);
    if (!kmsg)
        return -ENOMEM;

    /* Copia da área de usuário, verifica erros */
    ret = strncpy_from_user(kmsg, umsg, len + 1);
    if (ret < 0) {
        kfree(kmsg);
        return ret;
    }
    kmsg[len] = '\0';

    /* Imprime no log do kernel */
    printk(KERN_INFO "[printUserMessage] %s\n", kmsg);

    kfree(kmsg);
    return ret;  /* bytes copiados */
}