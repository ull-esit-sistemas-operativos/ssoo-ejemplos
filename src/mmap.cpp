// mmap.cpp - Ejemplo del uso de mmap() para mapear archivos

#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <cstdlib>
#include <cstring>
#include <iostream>

#define FILE_PERMISSIONS        S_IRUSR | S_IWUSR | \
                                S_IRGRP | S_IWGRP | \
                                S_IROTH | S_IWOTH       // rw-rw-rw-

int main(int argc, char* argv[])
{
    int fdin, fdout, result;
    char *src, *dst;
    struct stat statbuf;
    
    if (argc != 3) {
        printf("uso: %s <origen> <destino>\n", program_invocation_short_name);
        exit(2);
    }

    // abrir el archivo de origen
    fdin = open(argv[1], O_RDONLY);
    if (fdin < 0) {
        std::cerr << "fallo en open() al abrir el archivo de origen" << std::endl;
        exit(3);
    }

    // abrir/crear el archivo de destino
    fdout = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, FILE_PERMISSIONS);
    if (fdout < 0) {
        std::cerr << "fallo en open() al abrir el archivo de destino" << std::endl;
        exit(4);
    }

    // pedir el tamaño del archivo de origen
    result = fstat(fdin, &statbuf);
    if (result < 0) {
        std::cerr << "fallo en fstat()" << std::endl;
        exit(5);
    }

    // ir a la posición correspondiente al último byte en el archivo de destino
    result = lseek(fdout, statbuf.st_size - 1, SEEK_SET);
    if (result == -1) {
        std::cerr << "fallo en lseek()" << std::endl;
        exit(6);
    }

    // escribir un byte en la última posición para extender el archivo de destino
    result = write (fdout, "", 1);
    if (result != 1) {
        std::cerr << "fallo en write()" << std::endl;
        exit(7);
    }

    // mapear el archivo de origen
    src = (char*)mmap(NULL, statbuf.st_size, PROT_READ, MAP_SHARED, fdin, 0);
    if (src == MAP_FAILED) {
        std::cerr << "fallo en mmap() al archivo de origen" << std::endl;
        exit(8);
    }

    // mapear el archivo de destino
    dst = (char*)mmap(NULL, statbuf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fdout, 0);
    if (dst == MAP_FAILED) {
        std::cerr << "fallo en mmap() al archivo de destino" << std::endl;
        exit(9);
    }

    // copiar el archivo de origen en el archivo de destino
    memcpy(dst, src, statbuf.st_size);

    return 0;
}
