#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <errno.h>

#define SYS_PRINT_MSG 387  /* bate com syscall_32.tbl */

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <message>\n", argv[0]);
        return 1;
    }
    const char *msg = argv[1];
    size_t len = strlen(msg);
    long ret = syscall(SYS_PRINT_MSG, msg, len);
    if (ret < 0) {
        errno = -ret;
        perror("sys_printUserMessage");
        return 1;
    }
    printf("Printed %ld bytes to kernel log\n", ret);
    return 0;
}