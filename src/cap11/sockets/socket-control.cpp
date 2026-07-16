// socket-client.cpp - Programa de control del ejemplo del uso de sockets para comunicar procesos
//
//  El programa de ejemplo utiliza alarm() y las señales del sistema para mostrar periódicamente la hora. Además,
//  escucha en un socket de dominio UNIX al que puede mandar órdenes el programa de control.
//
//  Usamos sockets de dominio UNIX y no AF_INET por simplificar. Además usamos sockets no orientados a conexión
//  SOCK_DGRAM porque preservan la separación entre mensajes, lo que simplifica el ejemplo. Por defecto los socket
//  SOCK_DGRAM no son fiables (pueden perderse mensajes y desordenarlos) pero como los sockets de dominio UNIX son
//  locales, en la mayoría de las implementaciones son fiables.
//
//  Compilar:
//
//      g++ -o socket-control socket-control.cpp
//

#include <print>
#include <string>
#include <system_error>

#include <unistd.h>
#include <sys/socket.h> // Cabecera de sockets
#include <sys/types.h>
#include <sys/un.h>     // Cabecera de sockets de dominio UNIX

#include "socket-common.hpp"

int protected_main()
{
    // Crear un socket local para comunicarnos con el servidor.
    int sockfd = socket( AF_UNIX, SOCK_DGRAM, 0 );
    if (sockfd < 0)
    {
        throw std::system_error( errno, std::system_category(), "Fallo en socket()" );
    }

    std::println( "Cerrando el servidor..." );

    // Construir la dirección del socket del servidor en el formato que necesita sendto().
    sockaddr_un server_address = {};
    server_address.sun_family = AF_UNIX;
    CONTROL_SOCKET_NAME.copy( server_address.sun_path, sizeof(server_address.sun_path) - 1, 0 );

    // Enviar el comando de terminar al socket del servidor.
    ssize_t return_code = sendto( sockfd, QUIT_COMMAND.c_str(), QUIT_COMMAND.size(), 0,
        reinterpret_cast<sockaddr*>(&server_address), sizeof(server_address) );
    if (return_code < 0)
    {
        throw std::system_error( errno, std::system_category(), "Fallo en sendto()" );
    }

    close( sockfd );

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
