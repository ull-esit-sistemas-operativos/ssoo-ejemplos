// socket-server.cpp - Programa de ejemplo del uso de sockets para comunicar procesos
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
//      g++ -o socket socket.cpp ../common/timeserver.cpp
//

#include <print>
#include <string>
#include <system_error>

#include <unistd.h>
#include <sys/socket.h> // Cabecera de sockets
#include <sys/types.h>
#include <sys/un.h>     // Cabecera de sockets de dominio UNIX

#include "common/timeserver.hpp"
#include "socket-common.hpp"

const size_t MAX_COMMAND_SIZE = 100;

int protected_main()
{
    // Crear el socket local donde escuchar los comandos de control
    int sockfd = ::socket( AF_UNIX, SOCK_DGRAM, 0 );
    if (sockfd < 0)
    {
        throw std::system_error( errno, std::system_category(), "Fallo en socket()" );
    }

    // Crear la dirección del socket local
    sockaddr_un local_address = {};
    local_address.sun_family = AF_UNIX;
    CONTROL_SOCKET_NAME.copy( local_address.sun_path, sizeof(local_address.sun_path) );

    // Asignar la dirección al socket local
    int return_code = bind( sockfd, reinterpret_cast<sockaddr*>(&local_address), sizeof(local_address) );
    if (return_code < 0)
    {
        throw std::system_error( errno, std::system_category(), "Fallo en bind()" );
    }

    // Comenzar a mostrar la hora periódicamente.
    start_alarm();

    std::println( "Escuchando en el socket de control '{}'...", CONTROL_SOCKET_NAME );

    // Leer del socket los comandos e interpretarlos.
    bool quit_app = false;
    while (!quit_app)
    {
        std::array<char, MAX_COMMAND_SIZE> buffer;

        // Recibir un comando del socket
        ssize_t received_bytes = recv( sockfd, buffer.data(), buffer.size(), 0 );
        if (received_bytes < 0)
        {
            throw std::system_error( errno, std::system_category(), "Fallo en recv()" );
        }

        std::string_view received_command{ buffer.data(), static_cast<std::size_t>(received_bytes) };

        if (received_command == QUIT_COMMAND)
        {
            std::println( "Ha llegado orden de terminar ¡Adiós!" );
            quit_app = true;
        }
        // Aquí va código para detectar e interpretar más comandos...
        //
        else
        {
            std::println( "Comando de control no reconocido: '{}'", received_command );
        } 
    }

    // Parar de mostrar la hora periódicamente.
    stop_alarm();
    
    // Cerrar el socket local
    close( sockfd );
    // Eliminar el archivo especial que representa al socket local
    unlink( CONTROL_SOCKET_NAME.c_str() );

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
