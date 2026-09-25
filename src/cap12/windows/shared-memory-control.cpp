// shared-memory-control.cpp - Programa de control del ejemplo de comunicación mediante memoria compartida
//
//  El programa de ejemplo utiliza un temporizador del sistema para mostrar periódicamente la hora. Además, crea una
//  región de memoria compartida a la que puede unirse el programa de control para darle órdenes.
//
//  En general, los problemas de sincronización no son sencillos. Para simplicar, vamos a suponer que solo hay un
//  cliente conectado al mismo tiempo. Así solo necesitaremos dos semáforos para implementar la sincronización.
//
//  Es la versión con la API de Windows del ejemplo de ../posix/shared-memory-control.cpp
//
//  Compilar:
//
//      cl /std:c++latest /EHsc /utf-8 shared-memory-control.cpp
//

#include <print>
#include <string>
#include <system_error>

#include <windows.h>    // Cabecera principal de la API de Windows

#include "shared-memory-common.hpp"

int protected_main()
{
    // Abrir el objeto de memoria compartida con el nombre indicado en 'CONTROL_SHM_NAME'.
    //
    // Para abrir un objeto que ya existe hay una función distinta de la que lo crea, mientras que en POSIX se usa
    // shm_open() en los dos casos y lo que cambia son los flags.
    HANDLE shm_handle = OpenFileMappingA( FILE_MAP_ALL_ACCESS, FALSE, CONTROL_SHM_NAME.c_str() );
    if (shm_handle == nullptr)
    {
        throw std::system_error( static_cast<int>(GetLastError()), std::system_category(),
                                 "Fallo en OpenFileMapping()" );
    }

    // Reservar una región de la memoria virtual del tamaño de 'memory_content' y mapear en ella el objeto de memoria
    // compartida abierto.
    void* shared_mem = MapViewOfFile( shm_handle, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(memory_content) );
    if (shared_mem == nullptr)
    {
        CloseHandle( shm_handle );
        throw std::system_error( static_cast<int>(GetLastError()), std::system_category(), "Fallo en MapViewOfFile()" );
    }

    auto memory_region = static_cast<memory_content*>(shared_mem);

    // Abrir los semáforos que ha creado el servidor. En la versión de POSIX este paso no existe, porque los semáforos
    // están dentro de la memoria compartida y se obtienen al mapearla.
    HANDLE empty_semaphore = OpenSemaphoreA( SEMAPHORE_ALL_ACCESS, FALSE, EMPTY_SEMAPHORE_NAME.c_str() );
    HANDLE ready_semaphore = OpenSemaphoreA( SEMAPHORE_ALL_ACCESS, FALSE, READY_SEMAPHORE_NAME.c_str() );
    if (empty_semaphore == nullptr || ready_semaphore == nullptr)
    {
        throw std::system_error( static_cast<int>(GetLastError()), std::system_category(), "Fallo en OpenSemaphore()" );
    }

    std::println( "Canal de control abierto '{}'.", CONTROL_SHM_NAME );

    // Poner el proceso a la espera de que se pueda enviar un comando.
    WaitForSingleObject( empty_semaphore, INFINITE );

    std::println( "Cerrando el servidor..." );

    // Escribir el comando QUIT en la región de memoria compartida.
    QUIT_COMMAND.copy( memory_region->command_buffer.data(), memory_region->command_buffer.size() );
    memory_region->command_length = QUIT_COMMAND.size();

    // Indicar al servidor que ya se escribió el comando para que pueda leerlo.
    ReleaseSemaphore( ready_semaphore, 1, nullptr );

    std::println( "¡Adiós!" );

    // Liberar la región de memoria reservada para mapear el objeto de memoria compartida.
    UnmapViewOfFile( shared_mem );

    // Cerrar los manejadores de los semáforos y del objeto de memoria compartida.
    CloseHandle( empty_semaphore );
    CloseHandle( ready_semaphore );
    CloseHandle( shm_handle );

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
