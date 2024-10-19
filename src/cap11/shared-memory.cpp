// shared-memory.cpp - Ejemplo del uso de memoria compartida con nombre para comunicar procesos
//
//  El programa de ejemplo utiliza alarm() y las señales del sistema para mostrar periódicamente la hora. Además,
//  crea una región de memoria compartida a la que puede unirse el programa de control para darle órdenes.
//
//  En general, los problemas de sincronización no son sencillos. Para simplicar, vamos a suponer que solo hay un
//  cliente conectado al mismo tiempo. Así solo necesitaremos dos semáforos para implementar la sincronización.
//
//  Compilar:
//
//      g++ -I../ -lrt -o shared-memory shared-memory.cpp ../common/timeserver.c
//

#include <cerrno>       // La librería estándar de C está disponible tanto en cabeceras estilo <stdlib.h> como
#include <cstring>      // <cstdlib>. La primera es para usar con C, mientras que la segunda es la recomendada en C++
                        // pues mete las funciones en el espacio de nombres 'std', como el resto de la
                        // librería estándar de C++.
#include <print>
#include <string>
#include <string_view>
#include <system_error>

#include <unistd.h>     // Cabecera principal de la API POSIX del sistema operativo
#include <fcntl.h>
#include <sys/mman.h>   // Cabecera para mmap() y shm_open()    
#include <sys/types.h>
#include <sys/stat.h>

#include "common/timeserver.h"
#include "shared-memory-common.hpp"

int protected_main()
{
    // Crear el objeto de memoria compartida con el nombre indicado en 'CONTROL_SHM_NAME'.
    // Usamos el flag O_EXCL para que de un error si ya existe un objeto con el mismo nombre.
    int shm_fd = shm_open( CONTROL_SHM_NAME.c_str(), O_CREAT | O_EXCL | O_RDWR, 0666 );
    if (shm_fd < 0)
    {
        throw std::system_error( errno, std::system_category(), "Fallo en shm_open()" );
    }
    
    // El objeto de memoria compartida creado tiene tamaño 0.
    // Tenemos que extenderlo para que quepa la estructura 'memory_content'.
    if ( ftruncate( shm_fd, sizeof(struct memory_content) ) < 0 )
    {
        close( shm_fd );
        shm_unlink( CONTROL_SHM_NAME.c_str() );
        throw std::system_error( errno, std::system_category(), "Fallo en ftruncate()" );
    }

    // Reservar una región de la memoria virtual del tamaño de 'memory_content' y mapear en ella el objeto de memoria
    // compartida recientemente creado.
    void* shared_mem = mmap( nullptr, sizeof(memory_content), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0 );
    if (shared_mem == MAP_FAILED)
    {
        close( shm_fd );
        shm_unlink( CONTROL_SHM_NAME.c_str() );
        throw std::system_error( errno, std::system_category(), "Fallo en mmap()" );
    }

    auto memory_region = static_cast<memory_content*>(shared_mem);

    // Necesitamos un mecanismo para sincronizar este proceso y el de control. El de control necesita esperar a que el
    // servidor lea el último comando antes de escribir el siguiente. El servidor debe esperar a que el cliente ponga
    // un comando antes de intentar leer. Para esto el sistema ofrece mecanismos de sincronización, como los semáforos.
    //
    // Inicializar los semáforo.
    sem_init( &memory_region->empty, 1, 1);
    sem_init( &memory_region->ready, 1, 0);

    // Comenzar a mostrar la hora periódicamente.
    setup_signals();
    start_alarm();

    std::println( "Escuchando en el canal de control '{}'...", CONTROL_SHM_NAME);

    // Leer de la memoria compartida los comandos e interpretarlos.
    while (!quit_app)
    {
        // Poner el proceso a la espera de que esté el comando en la memoria compartida.
        sem_wait( &memory_region->ready );

        std::string_view received_command{ memory_region->command_buffer.data(), memory_region->command_length};
        
        if ( received_command == QUIT_COMMAND )
        {
            std::println( "Ha llegado orden de terminar ¡Adiós!" );
            quit_app = true;
        }

        // Aquí va código para detectar e interpretar más comandos...
        //                

        // Indicar al cliente que ya se leyó el comando, así que puede poner otro.
        sem_post( &memory_region->empty );
    }

    // Parar de mostrar la hora periódicamente.
    stop_alarm();

    // Liberar la región de memoria reservada para mapear el objeto de memoria compartida.
    munmap( shared_mem, sizeof(memory_content) );
    // Cerrar el descriptor de archivo del objeto de memoria compartida abierto.
    close( shm_fd );
    // Eliminar el objeto de memoria compartida. Nadie más podrá conectarse.
    shm_unlink( CONTROL_SHM_NAME.c_str() );

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