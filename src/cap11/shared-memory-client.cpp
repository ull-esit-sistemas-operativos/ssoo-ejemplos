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
//      g++ -lfmtlib -lrt -pthread -o shared-memory-client shared-memory-client.cpp
//

#include <cerrno>       // La librería estándar de C está disponible tanto en cabeceras estilo <stdlib.h> como
#include <cstring>      // <cstdlib>. La primera es para usar con C mientras quela segunda es la recomendada en C++
#include <iostream>     // pues mete las funciones en el espacio de nombres 'std', como el resto de la
#include <string>       // librería estándar de C++.
#include <system_error>

#include <fmt/core.h>   // Hasta que std::format (C++20) esté disponible

#include "shared-memory-server.h"

// Como no hay funciones para gestionar memoria compartida en C++, tenemos que usar directamente la librería del
// sistema. Abstrayendo su uso detrás de clases, simplificamos el resto del código del programa, facilitamos el manejo
// de errores y que todos los recursos se liberen. 

#include "shared_memory.hpp"

int protected_main()
{
    shared_memory control_shm;

    try
    {
        // Abrir el objeto de memoria compartida creado por el servidor.
        control_shm = shared_memory{ CONTROL_SHM_NAME };
    }
    catch ( const std::system_error& e )
    {
        if (e.code().value() == ENOENT)
        {
            std::cerr << "Error: El servidor no parece estar en ejecución.\n";
            return 1;
        }
        else throw;
    }

    // Reservar una región de la memoria virtual del proceso y mapear en ella el objeto de memoria compartida
    // recientemente creado.
    auto memory_region = control_shm.map<memory_content>( PROT_READ | PROT_WRITE );

    // Poner el proceso a la espera de que se pueda enviar un comando.
    sem_wait( &memory_region->empty );

    std::cout << "Cerrando el servidor...\n";

    // Escribir el comando en la región de memoria compartida.
    std::strcpy( memory_region->command_buffer, QUIT_COMMAND );

    // Indicar al servidor que ya se escribió el comando.
    sem_post( &memory_region->empty );

    std::cout << "¡Adiós!\n";

    return 0;
}

int main()
{
    try
    {
        return protected_main();
    }
    catch(std::system_error& e)
    {
        std::cerr << fmt::format( "Error ({}): {}\n", e.code().value(), e.what() );
    }
    catch(std::exception& e)
    {
        std::cerr << fmt::format( "Error: Excepción: {}\n", e.what() );
    }

    return 2;
}
