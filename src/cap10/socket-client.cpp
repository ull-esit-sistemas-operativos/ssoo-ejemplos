// socket-client.cpp - Cliente del ejemplo del uso de sockets para comunicar procesos
//
//  El programa servidor utiliza alarm() y las señales del sistema para mostrar periódicamente la hora. Además, escucha
//  en un socket de dominio UNIX al que puede mandar órdenes el cliente.
//
//  Usamos sockets de dominio UNIX y no AF_INET por simplificar. Además usamos sockets no orientados a conexión
//  SOCK_DGRAM porque preservan la separación entre mensajes, lo que simplifica el ejemplo. Por defecto los socket
//  SOCK_DGRAM no son fiables (pueden perderse mensajes y desordenarlos) pero como los sockets de dominio UNIX son
//  locales, en la mayoría de las implementaciones son fiables.
//
//  Compilar:
//
//      g++ -pthread -lfmtlib -o socket-client socket-client.cpp
//

#include <iostream>
#include <string>
#include <system_error>

#include <fmt/core.h>   // Hasta que std::format (C++20) esté disponible

#include "socket-server.hpp"

// Aunque se está trabajando en ello, en C++ no hay una librería de comunicaciones en red. Así que tenemos que usar
// directamente la librería del sistema. Abstrayendo su uso detrás de clases, simplificamos el resto del código del
// programa, facilitamos el manejo de errores y que todos los recursos se liberen. 

#include "socket.hpp"

int protected_main()
{
    // Crear un socket local para comunicarnos con el servidor.
    socket_t sock { socket_t::SOCKET_UNNAMED };

    std::cout << "Cerrando el servidor...\n";

    // Enviar el comando de terminar a la cola.
    sock.send( QUIT_COMMAND, CONTROL_SOCKET_NAME );

    std::cout << "¡Adiós!\n";

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
        std::cerr << fmt::format( "Error ({}): {}\n", e.code().value(), e.what() );
    }
    catch(std::exception& e)
    {
        std::cerr << fmt::format( "Error: Excepción: {}\n", e.what() );
    }

    return EXIT_FAILURE;
}
