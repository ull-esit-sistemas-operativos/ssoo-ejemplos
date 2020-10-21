// softstack.c - Ejemplo en C para mostrar la pila de software del sistema operativo
//
//  Compilar:
//
//      gcc -o softstack-c softstack.c
//
//  Uso:
//      # strace -e trace=open,openat,read,write,close ./softstack-c
//

#define _POSIX_C_SOURCE 200809L

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <stdio.h>
#include <stdlib.h>

int main()
{
    char filename[] = "/tmp/softstack-fileXXXXXX";

    mkstemp(filename);

    puts("Antes de abrir el archivo...");
    int fd = open(filename, O_RDWR | O_CREAT);
    puts("Después de abrir el archivo...");
    close(fd);

    return 0;
}
