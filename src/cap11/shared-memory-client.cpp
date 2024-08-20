// shared-memory-client.cpp - Cliente del ejemplo del uso de memoria compartida con nombre para comunicar procesos
//
//  El programa servidor utiliza alarm() y las señales del sistema para mostrar periódicamente la hora. Además, crea
//  una región de memoria compartia a la que puede unirseel programa cliente para darle órdenes.
//
//  Los problemas de sincronización no son sencillos. Para simplicar, vamos a suponer que solo hay un cliente conectado
//  al mismo tiempo. Así solo necesitaremos dos semáforos.
//
//  Compilar:
//
//      g++ -lrt -o shared-memory-client shared-memory-client.cpp
//

#include <cerrno>       // La librería estándar de C está disponible tanto en cabeceras estilo <stdlib.h> como
#include <cstring>      // <cstdlib>. La primera es para usar con C, mientras que la segunda es la recomendada en C++
                        // pues mete las funciones en el espacio de nombres 'std', como el resto de la
                        // librería estándar de C++.
#include <print>
#include <string>
#include <system_error>

#include "shared-memory-server.h"

// Como no hay funciones para gestionar memoria compartida en C++, tenemos que usar directamente la librería del
// sistema. Abstrayendo su uso detrás de clases, simplificamos el resto del código del programa, facilitamos el manejo
// de errores y que todos los recursos se liberen. 

#include "shared_memory.hpp"

int protected_main()
{
    examples::shared_memory control_shm;

    try
    {
        // Abrir el objeto de memoria compartida creado por el servidor.
        control_shm = examples::shared_memory{ CONTROL_SHM_NAME };
    }
    catch ( const std::system_error& e )
    {
        if (e.code().value() == ENOENT)
        {
            std::println( stderr, "Error: El servidor no parece estar en ejecución." );
            return EXIT_FAILURE;
        }
        else throw;
    }

    // Reservar una región de la memoria virtual del proceso y mapear en ella el objeto de memoria compartida
    // recientemente creado.
    auto memory_region = control_shm.map<memory_content>( PROT_READ | PROT_WRITE );

    // Poner el proceso a la espera de que se pueda enviar un comando.
    sem_wait( &memory_region->empty );

    std::println( "Cerrando el servidor..." );

    // Escribir el comando en la región de memoria compartida.
    std::strcpy( memory_region->command_buffer, QUIT_COMMAND );

    // Indicar al servidor que ya se escribió el comando.
    sem_post( &memory_region->empty );

    std::println( "¡Adiós!" );

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
