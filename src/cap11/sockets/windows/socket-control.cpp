// socket-control.cpp - Programa de control del ejemplo del uso de sockets para comunicar procesos
//
//  El programa de ejemplo utiliza un temporizador del sistema para mostrar periódicamente la hora. Además, escucha en
//  un socket al que puede mandar órdenes el programa de control.
//
//  Es la versión con la API de Windows del ejemplo de ../posix/socket-control.cpp
//
//  Compilar:
//
//      cl /std:c++latest /EHsc /utf-8 socket-control.cpp ws2_32.lib
//

#include <print>
#include <string>
#include <system_error>

// Cabecera de Winsock. Sus funciones están en la librería ws2_32.lib, con la que hay que enlazar el programa.
#include <winsock2.h>

#include "../socket-common.hpp"

int protected_main()
{
    // Inicializar Winsock, que en Windows es una librería que hay que inicializar antes de usarla.
    WSADATA wsa_data;
    int error_code = WSAStartup( MAKEWORD(2, 2), &wsa_data );
    if (error_code != 0)
    {
        throw std::system_error( error_code, std::system_category(), "Fallo en WSAStartup()" );
    }

    // Crear un socket local para comunicarnos con el servidor. No hace falta asignarle dirección con bind(), porque
    // solo lo usamos para enviar: el sistema operativo le asignará un puerto libre cualquiera al llamar a sendto().
    SOCKET sock = socket( AF_INET, SOCK_DGRAM, 0 );
    if (sock == INVALID_SOCKET)
    {
        throw std::system_error( WSAGetLastError(), std::system_category(), "Fallo en socket()" );
    }

    std::println( "Cerrando el servidor..." );

    // Construir la dirección del socket del servidor en el formato que necesita sendto().
    sockaddr_in server_address = {};
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl( INADDR_LOOPBACK );
    server_address.sin_port = htons( CONTROL_SOCKET_PORT );

    // Enviar el comando de terminar al socket del servidor.
    int return_code = sendto( sock, QUIT_COMMAND.c_str(), static_cast<int>(QUIT_COMMAND.size()), 0,
        reinterpret_cast<sockaddr*>(&server_address), sizeof(server_address) );
    if (return_code == SOCKET_ERROR)
    {
        throw std::system_error( WSAGetLastError(), std::system_category(), "Fallo en sendto()" );
    }

    closesocket( sock );
    WSACleanup();

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
