// shared-memory-control.cpp - Programa de control del ejemplo de comunicación mediante memoria compartida
//
//  El programa de ejemplo utiliza alarm() y las señales del sistema para mostrar periódicamente la hora. Además, crea
//  una región de memoria compartia a la que puede unirse el programa de control para darle órdenes.
//
//  En general, los problemas de sincronización no son sencillos. Para simplicar, vamos a suponer que solo hay un
//  cliente conectado al mismo tiempo. Así solo necesitaremos dos semáforos para implementar la sincronización.
//
//  Compilar:
//
//      g++ -lrt -o shared-memory-control shared-memory-control.cpp
//

#include <cerrno>       // La librería estándar de C está disponible tanto en cabeceras estilo <stdlib.h> como
#include <cstring>      // <cstdlib>. La primera es para usar con C, mientras que la segunda es la recomendada en C++
                        // pues mete las funciones en el espacio de nombres 'std', como el resto de la
                        // librería estándar de C++.
#include <print>
#include <string>
#include <system_error>

#include <unistd.h>     // Cabecera principal de la API POSIX del sistema operativo
#include <fcntl.h>
#include <sys/mman.h>   // Cabecera para mmap() y shm_open()
#include <sys/types.h>
#include <sys/stat.h>

#include "shared-memory-common.hpp"

int protected_main()
{
    // Abrir el objeto de memoria compartida el nombre indicado en 'CONTROL_SHM_NAME'.
    int shm_fd = shm_open( CONTROL_SHM_NAME.c_str(), O_RDWR, 0 );
    if (shm_fd < 0)
    {
        throw std::system_error( errno, std::system_category(), "Fallo en shm_open()" );
    }

    // Reservar una región de la memoria virtual del tamaño de 'memory_content' y mapear en ella el objeto de memoria
    // compartida abierto.
    void* shared_mem = mmap( nullptr, sizeof(memory_content), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0 );
    if (shared_mem == MAP_FAILED)
    {
        close( shm_fd );
        throw std::system_error( errno, std::system_category(), "Fallo en mmap()" );
    }

    auto memory_region = static_cast<memory_content*>(shared_mem);

    std::println( "Canal de control abierto '{}'.", CONTROL_SHM_NAME);

    // Poner el proceso a la espera de que se pueda enviar un comando.
    sem_wait( &memory_region->empty );

    std::println( "Cerrando el servidor..." );
    
    // Escribir el comando QUIT en la región de memoria compartida.
    QUIT_COMMAND.copy( memory_region->command_buffer.data(), memory_region->command_buffer.size() );
    memory_region->command_length = QUIT_COMMAND.size();

    // Indicar al servidor que ya se escribió el comando para que pueda leerlo.
    sem_post( &memory_region->ready );

    std::println( "¡Adiós!" );

    // Liberar la región de memoria reservada para mapear el objeto de memoria compartida.
    munmap( shared_mem, sizeof(memory_content) );
    // Cerrar el descriptor de archivo del objeto de memoria compartida abierto.
    close( shm_fd );

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
