// socket.cpp - Programa de ejemplo del uso de sockets para comunicar procesos
//
//  El programa de ejemplo utiliza alarm() y las señales del sistema para mostrar periódicamente la hora. Además,
//  escucha en un socket al que puede mandar órdenes el programa de control.
//
//  Usamos sockets AF_INET y no de dominio UNIX para que el ejemplo sea casi idéntico al de la API de Windows, que
//  solo admite sockets de dominio UNIX SOCK_STREAM. Además usamos sockets no orientados a conexión SOCK_DGRAM (UDP)
//  porque preservan la separación entre mensajes, lo que simplifica el ejemplo. UDP no es fiable (pueden perderse
//  mensajes y desordenarse) pero como el socket solo escucha en la interfaz de loopback, los mensajes nunca salen
//  del equipo y en la práctica no se pierden.
//
//  Compilar:
//
//      g++ -std=c++23 -I../../../ -o socket socket.cpp ../../../common/timeserver.cpp
//

#include <array>
#include <print>
#include <string>
#include <string_view>
#include <system_error>

#include <unistd.h>
#include <arpa/inet.h>  // Cabecera de htons() y htonl()
#include <netinet/in.h> // Cabecera de sockets AF_INET
#include <sys/socket.h> // Cabecera de sockets
#include <sys/types.h>

#include "common/timeserver.hpp"
#include "../socket-common.hpp"

const size_t MAX_COMMAND_SIZE = 100;

int protected_main()
{
    // Crear el socket local donde escuchar los comandos de control
    int sockfd = ::socket( AF_INET, SOCK_DGRAM, 0 );
    if (sockfd < 0)
    {
        throw std::system_error( errno, std::system_category(), "Fallo en socket()" );
    }

    // Crear la dirección del socket local: la dirección de loopback y el puerto de control.
    sockaddr_in local_address = {};
    local_address.sin_family = AF_INET;
    local_address.sin_addr.s_addr = htonl( INADDR_LOOPBACK );
    local_address.sin_port = htons( CONTROL_SOCKET_PORT );

    // Asignar la dirección al socket local
    int return_code = bind( sockfd, reinterpret_cast<sockaddr*>(&local_address), sizeof(local_address) );
    if (return_code < 0)
    {
        throw std::system_error( errno, std::system_category(), "Fallo en bind()" );
    }

    // Comenzar a mostrar la hora periódicamente.
    start_alarm();

    std::println( "Escuchando en el puerto UDP de control {}...", CONTROL_SOCKET_PORT );

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

    // Cerrar el socket local. El puerto queda libre para que otro proceso lo use.
    close( sockfd );

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
