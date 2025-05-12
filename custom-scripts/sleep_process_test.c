#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <string.h>
#include <errno.h>

#define SYS_listSleepingProcesses 386  // Número da syscall, certifique-se de que seja o correto

int main() {
    char buffer[4096];  // Aumente o buffer para garantir que seja grande o suficiente
    long ret;

    memset(buffer, 0, sizeof(buffer));  // Limpa o buffer antes de usá-lo

    // Chama a syscall para listar os processos em estado de sleep
    ret = syscall(SYS_listSleepingProcesses, buffer, sizeof(buffer));
    
    if (ret < 0) {
        perror("syscall listSleepingProcesses");  // Mostra o erro, caso a syscall falhe
        return 1;
    }

    // Exibe os processos que estão em estado de sleep
    printf("Processos em estado de sleep:\n%s\n", buffer);
    return 0;
}
