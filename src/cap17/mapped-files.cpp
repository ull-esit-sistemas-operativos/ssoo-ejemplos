// mapped-files.cpp - Ejemplo en C++ del uso de mmap() para mapear archivos
//
//  El programa mapea el archivo indicado por la línea de comandos en la memoria y calcula el el número de líneas,
//  palabras y caracteres, como hace el comando wc.
//
//  Compilar:
//
//      g++ -o mapped-files-cpp mapped-files.cpp
//

#include <algorithm>
#include <print>
#include <system_error>

#include <fcntl.h>
#include <libgen.h>     // Cabecera para basename()
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

int protected_main(int argc, char* argv[])
{
    char* program_name = basename( argv[0] );

    if (argc != 2)
    {
        std::println( stderr, "Uso: {} <archivo>", program_name );
        return EXIT_FAILURE;
    }
    
    // Abrir el archivo que se quiere mapear en memoria.
    int fd = open( argv[1], O_RDONLY );
    if (fd == -1)
    {
        throw std::system_error( errno, std::system_category(), "Fallo en open()" );
    }

    // Obtener el tamaño del archivo.
    // La función lseek() sirve para mover el puntero de lectura/escritura de un archivo y retorna la posición
    // a la que se ha movido. Si se mueve al final del archivo, se obtiene el tamaño del archivo.
    int file_size = lseek( fd, 0, SEEK_END );

    // Mapear `length` bytes del archivo desde el byte 0 en la memoria del proceso, solo para lectura.
    // Como `length` es el tamaño del archivo, se mapea todo el archivo.
    void* mapped_file = mmap( nullptr, file_size, PROT_READ, MAP_SHARED, fd, 0 );
    if (mapped_file == MAP_FAILED)
    {
        throw std::system_error( errno, std::system_category(), "Fallo en mmap()" );
    }

    // Cerar el descriptor de archivo, ya que no se necesita más.
    close( fd );

    const uint8_t* memory_region_begin = static_cast<uint8_t*>(mapped_file);
    const uint8_t* memory_region_end = memory_region_begin + file_size;
    size_t lines = 0, words = 0, characters = 0;
    bool space_character = true;

    // Contar líneas, palabras y caracteres
    for (const uint8_t* p = memory_region_begin; p < memory_region_end; ++p)
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

    std::println( "{: >7} {: >7} {: >7}", lines, words, characters);

    return EXIT_SUCCESS;
}

int main(int argc, char* argv[])
{
    try
    {
        return protected_main(argc, argv);
    }
    catch(std::system_error& e)
    {
        std::println( stderr, "Error ({}): {}", e.code().value(), e.what() );
    }
    catch(std::exception& e)
    {
        std::println( stderr, "Error: Excepción: {}", e.what() );
    }

    return EXIT_FAILURE;
}