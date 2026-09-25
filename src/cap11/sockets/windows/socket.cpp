// socket.cpp - Programa de ejemplo del uso de sockets para comunicar procesos
//
//  El programa de ejemplo utiliza un temporizador del sistema para mostrar periódicamente la hora. Además, escucha en
//  un socket al que puede mandar órdenes el programa de control.
//
//  Es la versión con la API de Windows del ejemplo de ../posix/socket.cpp
//
//  La interfaz de sockets de Windows, Winsock, copia la interfaz de sockets de BSD, que es la misma que la de POSIX,
//  así que el código es casi igual. Las diferencias están comentadas donde aparecen.
//
//  Compilar:
//
//      cl /std:c++latest /EHsc /utf-8 /I..\..\.. socket.cpp ..\..\..\common\timeserver-windows.cpp ws2_32.lib
//

#include <array>
#include <print>
#include <string>
#include <string_view>
#include <system_error>

// Cabecera de Winsock. Sus funciones están en la librería ws2_32.lib, con la que hay que enlazar el programa.
#include <winsock2.h>

#include <common/timeserver.hpp>
#include "../socket-common.hpp"

const int MAX_COMMAND_SIZE = 100;

int protected_main()
{
    // Inicializar Winsock.
    //
    // Este paso no existe en POSIX, donde los sockets son parte del sistema operativo, como los archivos. En Windows
    // son una librería que hay que inicializar antes de usar cualquier otra de sus funciones, indicando la versión de
    // Winsock que queremos, la 2.2. Además, WSAStartup() no deja el código de error en ningún sitio: lo devuelve.
    WSADATA wsa_data;
    int error_code = WSAStartup( MAKEWORD(2, 2), &wsa_data );
    if (error_code != 0)
    {
        throw std::system_error( error_code, std::system_category(), "Fallo en WSAStartup()" );
    }

    // Crear el socket local donde escuchar los comandos de control.
    //
    // En POSIX socket() devuelve un descriptor de archivo, un int que es -1 si hay error. Aquí devuelve un SOCKET, un
    // entero sin signo del tamaño de un puntero, así que el error se indica con el valor especial INVALID_SOCKET.
    // Tampoco se usa errno, sino WSAGetLastError(), para obtener el código del error.
    SOCKET sock = socket( AF_INET, SOCK_DGRAM, 0 );
    if (sock == INVALID_SOCKET)
    {
        throw std::system_error( WSAGetLastError(), std::system_category(), "Fallo en socket()" );
    }

    // Crear la dirección del socket local: la dirección de loopback y el puerto de control.
    sockaddr_in local_address = {};
    local_address.sin_family = AF_INET;
    local_address.sin_addr.s_addr = htonl( INADDR_LOOPBACK );
    local_address.sin_port = htons( CONTROL_SOCKET_PORT );

    // Asignar la dirección al socket local. En Winsock las funciones que en POSIX devuelven -1 si hay error,
    // devuelven SOCKET_ERROR, que también vale -1.
    int return_code = bind( sock, reinterpret_cast<sockaddr*>(&local_address), sizeof(local_address) );
    if (return_code == SOCKET_ERROR)
    {
        throw std::system_error( WSAGetLastError(), std::system_category(), "Fallo en bind()" );
    }

    // Comenzar a mostrar la hora periódicamente.
    start_alarm();

    std::println( "Escuchando en el puerto UDP de control {}...", CONTROL_SOCKET_PORT );

    // Leer del socket los comandos e interpretarlos.
    bool quit_app = false;
    while (!quit_app)
    {
        std::array<char, MAX_COMMAND_SIZE> buffer;

        // Recibir un comando del socket. En Winsock el buffer es un char* y su tamaño un int, no un void* y un size_t,
        // y el resultado es un int, porque en Windows no existe el tipo ssize_t.
        int received_bytes = recv( sock, buffer.data(), MAX_COMMAND_SIZE, 0 );
        if (received_bytes == SOCKET_ERROR)
        {
            throw std::system_error( WSAGetLastError(), std::system_category(), "Fallo en recv()" );
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

    // Cerrar el socket local. Un SOCKET no es un descriptor de archivo, así que no se cierra con close() sino con
    // closesocket().
    closesocket( sock );

    // Liberar los recursos de Winsock, que se reservaron en WSAStartup().
    WSACleanup();

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
