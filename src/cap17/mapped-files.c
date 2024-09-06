// mapped-files.c - Ejemplo en C del uso de mmap() para mapear archivos
//
//  El programa mapea el archivo indicado por la línea de comandos en la memoria y calcula el el número de líneas,
//  palabras y caracteres, como hace el comando wc.
//
//  Compilar:
//
//      gcc -o mapped-files mapped-files.c
//

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>     // Cabecera principal de la API POSIX del sistema operativo
#include <fcntl.h>      // Cabecera para open()
#include <libgen.h>     // Cabecera para basename()
#include <sys/mman.h>   // Cabecera para mmap()
#include <sys/types.h>
#include <sys/stat.h>

void memory_word_count(uint8_t* first_char_ptr, uint8_t* last_char_ptr)
{
    size_t lines = 0, words = 0, characters = 0;
    bool space_character = true;

    // Contar líneas, palabras y caracteres
    for (const uint8_t* p = first_char_ptr; p < last_char_ptr; ++p)
    {
        if (*p == '\n') lines++;
        if (*p == '\n' || *p == '\t' || *p == ' ')
        {
            if (! space_character) words++;
            space_character = true;   
        }
        else
        {
            space_character = false;
        }
        characters++;
    }

    printf( "%7ld %7ld %7ld\n", lines, words, characters);
}

int main(int argc, char* argv[])
{
    char* program_name = basename( argv[0] );

    int fd = -1;
    uint8_t* mapped_mem = MAP_FAILED;
    struct stat statbuf;

    if (argc != 2)
    {
        fprintf( stderr, "Uso: %s <archivo>\n", program_name);
        return EXIT_FAILURE;
    }
    
    fd = open( argv[1], O_RDONLY );
    if (fd < 0)
    {
        fprintf( stderr, "Error (%d) al abrir el archivo: %s\n", errno, strerror(errno) );
        return EXIT_FAILURE;
    }

    int stat_return_code = fstat(fd, &statbuf);
    if (stat_return_code >= 0)
    {
        mapped_mem = mmap( NULL, statbuf.st_size, PROT_READ, MAP_SHARED, fd, 0 );
    }

    if (mapped_mem == MAP_FAILED)
    {
        fputs( "Error al mapear el archivo en memoria.\n", stderr);

        // Vamos a salir del programa...
        close(fd);
        return EXIT_FAILURE;
    }

    memory_word_count(mapped_mem, mapped_mem + statbuf.st_size);

    // Vamos a salir del programa...
    munmap(mapped_mem, statbuf.st_size);
    close(fd);

    return EXIT_SUCCESS;
}