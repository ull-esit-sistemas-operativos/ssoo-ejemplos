// softstack.c - Ejemplo para mostrar la pila de software del sistema
//               operativo
//
//      gcc -o softstack softstack.cpp
//
//      uso:
//              # ltrace -S -C -n4 ./softstack
//

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

int main()
{
    char filename[] = "/tmp/softstack-fileXXXXXX";
    mkstemp(filename);

    puts("antes de abrir el archivo...");
    int fd = open(filename, O_RDWR | O_CREAT);
    puts("después de abrir el archivo...");
    close(fd);

    return 0;
}
