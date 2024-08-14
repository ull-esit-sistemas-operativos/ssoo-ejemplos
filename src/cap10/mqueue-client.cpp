// mqueue-client.cpp - Cliente del ejemplo del uso de colas de mensajes para comunicar procesos
//
//  El programa servidor utiliza alarm() y las señales del sistema para mostrar periódicamente la hora. Además, crea
//  una cola de mensajes a la que puede mandar órdenes el cliente.
//
//  Compilar:
//
//      g++ -lrt -o mqueue-client mqueue-client.cpp
//

#include <cerrno>
#include <print>
#include <string>
#include <system_error>

#include "mqueue-server.hpp"

// Como no hay funciones para usar las colas de mensajes en C++, tenemos que usar directamente la librería del sistema.
// Abstrayendo su uso detrás de clases, simplificamos el resto del código del programa, facilitamos el manejo de
// errores y que todos los recursos se liberen. 

#include "message_queue.hpp"

int protected_main()
{
    message_queue control_queue;

    try
    {
        // Abrir la cola de mensajes creada por el servidor.
        control_queue = message_queue{ CONTROL_QUEUE_NAME, message_queue::open_mode::write_only };
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

    std::println( "Cerrando el servidor..." );

    // Enviar el comando de terminar a la cola.
    control_queue.send( QUIT_COMMAND, 0 );

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
