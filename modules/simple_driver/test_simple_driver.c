#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define DEVICE_FILE "/dev/simple_driver"

int main() {
    int fd;
    char buffer[256];

    // Abre o dispositivo
    fd = open(DEVICE_FILE, O_RDWR);
    if (fd == -1) {
        perror("Erro ao abrir o dispositivo");
        return -1;
    }

    // Escreve uma mensagem no driver
    write(fd, "Mensagem 1", strlen("Mensagem 1"));
    write(fd, "Mensagem 2", strlen("Mensagem 2"));

    // Lê a mensagem do driver
    read(fd, buffer, sizeof(buffer));
    printf("Mensagem lida: %s\n", buffer);

    // Fecha o dispositivo
    close(fd);

    return 0;
}
