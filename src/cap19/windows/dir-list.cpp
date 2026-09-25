// dir-list.cpp - Listar el contenido de un directorio
//
//  Este programa lista el contenido de un directorio. Si no se especifica un directorio, lista el contenido del
//  directorio actual.
//
//  Es la versión con la API de Windows del ejemplo de ../posix/dir-list.cpp
//
//  Compilar:
//      cl /std:c++latest /EHsc /utf-8 dir-list.cpp
//

#include <print>
#include <string>
#include <system_error>

#include <windows.h>    // Cabecera principal de la API de Windows

int protected_main(int argc, char* argv[])
{
    const char* dir_name = ".";
    if (argc > 1)
    {
        dir_name = argv[1];
    }

    std::println( "Listando el contenido de '{}':", dir_name );

    // Abrir el directorio.
    //
    // FindFirstFile() no recibe el nombre de un directorio, como opendir(), sino un patrón de búsqueda, así que hay
    // que añadirle '\*' para que devuelva todas las entradas.
    //
    // Además hace a la vez el trabajo de opendir() y el de la primera llamada a readdir(): junto al manejador con el
    // que seguir recorriendo el directorio, devuelve la primera entrada. Por eso el bucle es 'do ... while' y no
    // 'while', como en la versión de POSIX.
    std::string search_pattern = std::string( dir_name ) + "\\*";

    WIN32_FIND_DATAA find_data;
    HANDLE dir_handle = FindFirstFileA( search_pattern.c_str(), &find_data );
    if (dir_handle == INVALID_HANDLE_VALUE)
    {
        throw std::system_error( static_cast<int>(GetLastError()), std::system_category(),
                                 "Fallo en FindFirstFile()" );
    }

    // Leer el contenido del directorio.
    do
    {
        std::println( "{}", find_data.cFileName );
    }
    while (FindNextFileA( dir_handle, &find_data ));

    // FindNextFile() devuelve falso tanto cuando termina de recorrer el directorio como cuando hay un error, así
    // que hay que preguntarle al sistema cuál de las dos cosas ha pasado. En POSIX readdir() distingue ambos casos
    // dejando errno a cero al llegar al final del directorio.
    DWORD last_error = GetLastError();
    if (last_error != ERROR_NO_MORE_FILES)
    {
        throw std::system_error( static_cast<int>(last_error), std::system_category(), "Fallo en FindNextFile()" );
    }

    // Cerrar el directorio.
    if (! FindClose( dir_handle ))
    {
        throw std::system_error( static_cast<int>(GetLastError()), std::system_category(), "Fallo en FindClose()" );
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
