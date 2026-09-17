// softstack.c - Ejemplo en C para mostrar la pila de software del sistema operativo
//
//  Compilar:
//
//      gcc -std=c11 -o softstack-c softstack.c
//
//  Uso:
//
//      ltrace -CS -n4 ./softstack-c
//

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

int main()
{
    char filename[] = "/tmp/softstack-fileXXXXXX";

    mkstemp(filename);

    puts("Antes de abrir el archivo...");
    int fd = open(filename, O_RDWR | O_CREAT);
    puts("Después de abrir el archivo...");
    close(fd);

    return EXIT_SUCCESS;
}
