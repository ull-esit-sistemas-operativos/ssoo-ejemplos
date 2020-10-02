// socket-server.cpp - Servidor del ejemplo del uso de socketspara comunicar procesos
//
//  El programa servidor utiliza alarm() y las señales del sistema para mostrar periódicamente
//  la hora. Además, esucha en un socket de dominio UNIX al que puede mandar órdenes el cliente.
//
//  Usamos sockets de dominio UNIX y no AF_INET por simplificar. Además usamos sockets no orientados
//  a conexión SOCK_DGRAM porque preservan la separación entre mensajes, lo que simplifica el
//  ejemplo. Por defecto los socket SOCK_DGRAM no son fiables (pueden perderse mensajes y
//  desordenarlos) pero como los sockets de dominio UNIX son locales, en la mayoría de las
//  implementaciones son fiables.
//
//  Compilar:
//
//      g++ -pthread -lfmtlib -o socket-server socket-server.cpp ../common/timeserver.c
//

#include <iostream>
#include <string>
#include <system_error>

#include <fmt/core.h>   // Hasta que std::format (C++20) esté disponible

#include "common/timeserver.h"
#include "socket-server.hpp"

// Aunque se está trabajando en ello, en C++ no hay una librería de comunicaciones en red. Así que
// tenemos que usar directamente la librería del sistema. Abstrayendo su uso detrás de clases,
// simplificamos el resto del código del programa, facilitamos el mamejo de errores y que todos
// los recursos se liberen. 

#include "socket.hpp"

int protected_main()
{
    socket_t sock;

    try
    {
        // Crear el socket local donde escuchar los comandos de control
        sock = socket_t{ CONTROL_SOCKET_NAME };
    }
    catch ( const std::system_error& e )
    {
        if (e.code().value() == EADDRINUSE)
        {
            std::cerr << "Error: Hay otro servidor en ejecución.\n";
            return 1;
        }
        else throw;
    }

    setup_signals();
    start_alarm();

    std::cout << fmt::format( "Escuchando en el canal de control '{}'...\n", CONTROL_SOCKET_NAME );

    // Leer del socket los comandos e interpretarlos.
    while (!quit_app)
    {
        // Poner el proceso a la espera de que llegue un comando
        auto message = sock.receive();

        if (message == QUIT_COMMAND)
        {
            quit_app = true;
        }

        // Aquí va código para detectar e interpretar más comandos...
        //                
    }

    stop_alarm();
    
    // Vamos a salir del programa...
    std::cout << "Ha llegado orden de terminar ¡Adios!\n";

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
