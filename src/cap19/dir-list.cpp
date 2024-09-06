// dir-list.cpp - Listar el contenido de un directorio
//
//  Este programa lista el contenido de un directorio. Si no se especifica un directorio, lista el contenido del
//  directorio actual.
//
//  Compilar:
//      g++ -o dir-list dir-list.cpp
//

#include <dirent.h>
#include <unistd.h>

#include <print>
#include <string>
#include <system_error>
#include <vector>

int protected_main(int argc, char* argv[])
{
    const char* dir_name = ".";
    if (argc > 1)
    {
        dir_name = argv[1];
    }

    std::println( "Listando el contenido de '{}':", dir_name );

    // Abrir el directorio.
    DIR* dir = opendir( dir_name );
    if (dir == nullptr)
    {
        throw std::system_error( errno, std::system_category(), "Fallo en opendir()" );
    }

    // Leer el contenido del directorio.
    struct dirent* entry;
    while ((entry = readdir( dir )) != nullptr)
    {
        std::println( "{}", entry->d_name );
    }

    // Cerrar el directorio.
    if (closedir( dir ) == -1)
    {
        throw std::system_error( errno, std::system_category(), "Fallo en closedir()" );
    }

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