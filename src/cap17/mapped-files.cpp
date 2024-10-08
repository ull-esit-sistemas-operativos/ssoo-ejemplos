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

#include <libgen.h>     // Cabecera para basename()

// Como no hay funciones para gestionar ficheros mapeados en memoria en C++, tenemos que usar directamente la librería
// del sistema. Abstrayendo su uso detrás de clases, simplificamos el resto del código del programa, facilitamos el
// manejo de errores y que todos los recursos se liberen. 

#include "memory_map.hpp"

int protected_main(int argc, char* argv[])
{
    char* program_name = basename( argv[0] );

    if (argc != 2)
    {
        std::println( stderr, "Uso: {} <archivo>", program_name );
        return EXIT_FAILURE;
    }
    
    // Abrir el archivo que se quiere mapear en memoria.
    examples::memory_map mapped_file { argv[1], examples::memory_map::open_mode::read_only };

    // Reservar una región de la memoria virtual del proceso y mapear en ella el archivo.
    struct stat statinfo = mapped_file.status();
    auto memory_region =  mapped_file.map<uint8_t>( PROT_READ, statinfo.st_size );

    const uint8_t* memory_region_begin = memory_region.get();
    const uint8_t* memory_region_end = memory_region_begin + statinfo.st_size;
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