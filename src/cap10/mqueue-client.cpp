// mqueue-client.cpp - Cliente del ejemplo del uso de colas de mensajes para comunicar
//                     procesos
//
//  El programa servidor utiliza alarm() y las señales del sistema para mostrar periódicamente
//  la hora. Además, crea una cola de mensajes a la que puede mandar órdenes el cliente.
//
//  Compilar:
//
//      g++ -pthread -lfmtlib -lrt -o mqueue-client mqueue-client.cpp
//

#include <cerrno>
#include <iostream>
#include <string>
#include <system_error>

#include <fmt/core.h>   // Hasta que std::format (C++20) esté disponible

#include "mqueue-server.hpp"

// Como no hay funciones para usar las colas de mensajes en C++, tenemos que usar directamente la
// librería del sistema. Abstrayendo su uso detrás de clases, simplificamos el resto del código del
// programa, facilitamos el mamejo de errores y que todos los recursos se liberen. 

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
            std::cerr << "Error: El servidor no parece estar en ejecución.\n";
            return 1;
        }
        else throw;
    }

    std::cout << "Cerrando el servidor...\n";

    // Enviar el comando de terminar a la cola.
    control_queue.send( QUIT_COMMAND, 0 );

    std::cout << "¡Adios!\n";

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
	    return 2;
    }
}
