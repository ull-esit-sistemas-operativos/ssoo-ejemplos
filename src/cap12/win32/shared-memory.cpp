// shared-memory.cpp - Ejemplo del uso de memoria compartida con nombre para comunicar procesos
//
//  El programa de ejemplo utiliza un temporizador del sistema para mostrar periódicamente la hora. Además, crea una
//  región de memoria compartida a la que puede unirse el programa de control para darle órdenes.
//
//  En general, los problemas de sincronización no son sencillos. Para simplicar, vamos a suponer que solo hay un
//  cliente conectado al mismo tiempo. Así solo necesitaremos dos semáforos para implementar la sincronización.
//
//  Es la versión con la API Win32 del ejemplo de ../posix/shared-memory.cpp
//
//  Compilar:
//
//      cl /std:c++latest /EHsc /utf-8 /I..\.. shared-memory.cpp ..\..\common\timeserver-win32.cpp
//

#include <print>
#include <string>
#include <string_view>
#include <system_error>

#include <windows.h>    // Cabecera principal de la API Win32 del sistema operativo

#include <common/timeserver.hpp>
#include "shared-memory-common.hpp"

int protected_main()
{
    // Crear el objeto de memoria compartida con el nombre indicado en 'CONTROL_SHM_NAME'.
    //
    // En POSIX el objeto de memoria compartida se crea con shm_open(), que devuelve un descriptor de archivo al que
    // hay que darle tamaño con ftruncate(). Aquí se hace todo de una vez: al pasar INVALID_HANDLE_VALUE como
    // archivo, la región no respalda a ningún archivo del disco sino al archivo de paginación del sistema.
    HANDLE shm_handle = CreateFileMappingA( INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0,
                                            sizeof(memory_content), CONTROL_SHM_NAME.c_str() );
    if (shm_handle == nullptr)
    {
        throw std::system_error( static_cast<int>(GetLastError()), std::system_category(),
                                 "Fallo en CreateFileMapping()" );
    }

    // CreateFileMapping() no falla si ya existe un objeto con ese nombre: devuelve un manejador del que ya había.
    // Para conseguir el efecto del flag O_EXCL de la versión de POSIX hay que preguntar qué ha pasado.
    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        CloseHandle( shm_handle );
        throw std::runtime_error( "Ya hay otro servidor en ejecución." );
    }

    // Reservar una región de la memoria virtual del tamaño de 'memory_content' y mapear en ella el objeto de memoria
    // compartida recientemente creado.
    void* shared_mem = MapViewOfFile( shm_handle, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(memory_content) );
    if (shared_mem == nullptr)
    {
        CloseHandle( shm_handle );
        throw std::system_error( static_cast<int>(GetLastError()), std::system_category(), "Fallo en MapViewOfFile()" );
    }

    auto memory_region = static_cast<memory_content*>(shared_mem);

    // Necesitamos un mecanismo para sincronizar este proceso y el de control. El de control necesita esperar a que el
    // servidor lea el último comando antes de escribir el siguiente. El servidor debe esperar a que el cliente ponga
    // un comando antes de intentar leer. Para esto el sistema ofrece mecanismos de sincronización, como los semáforos.
    //
    // Crear los semáforos.
    //
    // Los de POSIX se inicializan con sem_init() dentro de la propia memoria compartida, que es lo que hace que los
    // dos procesos vean el mismo. Los de la API Win32 son objetos del sistema, como la región de memoria, y se
    // comparten dándoles un nombre.
    HANDLE empty_semaphore = CreateSemaphoreA( nullptr, 1, 1, EMPTY_SEMAPHORE_NAME.c_str() );
    HANDLE ready_semaphore = CreateSemaphoreA( nullptr, 0, 1, READY_SEMAPHORE_NAME.c_str() );
    if (empty_semaphore == nullptr || ready_semaphore == nullptr)
    {
        throw std::system_error( static_cast<int>(GetLastError()), std::system_category(),
                                 "Fallo en CreateSemaphore()" );
    }

    // Comenzar a mostrar la hora periódicamente.
    start_alarm();

    std::println( "Escuchando en el canal de control '{}'...", CONTROL_SHM_NAME );

    // Leer de la memoria compartida los comandos e interpretarlos.
    bool quit_app = false;
    while (!quit_app)
    {
        // Poner el proceso a la espera de que esté el comando en la memoria compartida.
        WaitForSingleObject( ready_semaphore, INFINITE );

        std::string_view received_command{ memory_region->command_buffer.data(), memory_region->command_length };

        if ( received_command == QUIT_COMMAND )
        {
            std::println( "Ha llegado orden de terminar ¡Adiós!" );
            quit_app = true;
        }

        // Aquí va código para detectar e interpretar más comandos...
        //

        // Indicar al cliente que ya se leyó el comando, así que puede poner otro.
        ReleaseSemaphore( empty_semaphore, 1, nullptr );
    }

    // Parar de mostrar la hora periódicamente.
    stop_alarm();

    // Liberar la región de memoria reservada para mapear el objeto de memoria compartida.
    UnmapViewOfFile( shared_mem );

    // Cerrar los manejadores del objeto de memoria compartida y de los semáforos.
    //
    // Aquí no hace falta nada parecido al shm_unlink() de la versión de POSIX. Los objetos del sistema con nombre
    // desaparecen solos en cuanto se cierra el último manejador que les quedaba abierto, mientras que los objetos de
    // memoria compartida de POSIX se quedan en el sistema hasta que alguien los borra.
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
