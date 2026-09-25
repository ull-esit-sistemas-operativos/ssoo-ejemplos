// namedpipe-control.cpp - Programa de control del ejemplo del uso de tuberías con nombre para comunicar procesos
//
//  El programa de ejemplo utiliza un temporizador del sistema para mostrar periódicamente la hora. Además, crea una
//  tubería con nombre a la que puede conectarse el programa de control para darle órdenes.
//
//  Es la versión con la API de Windows del ejemplo de ../posix/fifo-control.cpp
//
//  Compilar:
//
//      cl /std:c++latest /EHsc /utf-8 namedpipe-control.cpp
//

#include <print>
#include <string>
#include <system_error>

#include <windows.h>    // Cabecera principal de la API de Windows

#include "namedpipe-common.hpp"

int protected_main()
{
    // Abrir la tubería usando su nombre, como un archivo convencional.
    //
    // Al cliente la tubería con nombre se le parece mucho más a un archivo que al servidor: la abre con la misma
    // función que cualquier otro, CreateFile(). La versión de POSIX aprovecha ese mismo parecido para abrirla con un
    // std::ofstream de la librería estándar de C++, sin usar la API del sistema para nada.
    HANDLE pipe_handle = CreateFileA( CONTROL_PIPE_NAME.c_str(), GENERIC_WRITE, 0, nullptr, OPEN_EXISTING,
                                      FILE_ATTRIBUTE_NORMAL, nullptr );
    if (pipe_handle == INVALID_HANDLE_VALUE)
    {
        // Al contrario que con los flujos de C++, aquí sí se puede saber por qué no se pudo abrir la tubería. Si el
        // error es ERROR_FILE_NOT_FOUND, por ejemplo, es que no existe y seguramente el servidor no esté en marcha.
        throw std::system_error( static_cast<int>(GetLastError()), std::system_category(), "Fallo en CreateFile()" );
    }

    std::println( "Cerrando el servidor..." );

    // Escribir el comando en la tubería, con el salto de línea del final que espera el servidor.
    std::string command = QUIT_COMMAND + "\n";

    DWORD bytes_written;
    if (! WriteFile( pipe_handle, command.c_str(), static_cast<DWORD>(command.length()), &bytes_written, nullptr ))
    {
        CloseHandle( pipe_handle );
        throw std::system_error( static_cast<int>(GetLastError()), std::system_category(), "Fallo en WriteFile()" );
    }

    std::println( "¡Adiós!" );

    // Cerrar la tubería. Así el servidor sabe que este cliente ya no tiene nada más que decir.
    CloseHandle( pipe_handle );

    return EXIT_SUCCESS;
}

int main()
{
    try
    {
        return protected_main();
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
