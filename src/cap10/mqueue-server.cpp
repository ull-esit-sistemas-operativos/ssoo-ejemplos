// mqueue-server.cpp - Servidor del ejemplo del uso de colas de mensajes para comunicar procesos
//
//  El programa servidor utiliza alarm() y las señales del sistema para mostrar periódicamente la hora. Además, crea
//  una cola de mensajes a la que puede mandar órdenes el cliente.
//
//  Compilar:
//
//      g++ -lrt -o mqueue-server mqueue-server.cpp ../common/timeserver.c
//

#include <print>
#include <string>
#include <system_error>

#include "common/timeserver.h"
#include "mqueue-server.hpp"

// Como no hay funciones para usar los colas de mensajes en C++, tenemos que usar directamente la librería del sistema.
// Abstrayendo su uso detrás de clases, simplificamos el resto del código del programa, facilitamos el manejo de
// errores y que todos los recursos se liberen. 

#include "message_queue.hpp"

int protected_main()
{
    message_queue control_queue;

    try
    {
        // Crear la cola de mensajes donde escuchar los comandos de control
        control_queue = message_queue{ CONTROL_QUEUE_NAME, message_queue::open_mode::read_only, true };
    }
    catch ( const std::system_error& e )
    {
        if (e.code().value() == EEXIST)
        {
            std::println( stderr, "Error: Hay otro servidor en ejecución." );
            return EXIT_FAILURE;
        }
        else throw;
    }

    setup_signals();
    start_alarm();

    std::println( "Escuchando en el canal de control '{}'...", CONTROL_QUEUE_NAME );

    // Leer de la cola de mensajes los comandos e interpretarlos.
    while (!quit_app)
    {
        try
        {
            // Poner el proceso a la espera de que llegue un comando
            auto [message, prio] = control_queue.receive();

            if (message == QUIT_COMMAND)
            {
                quit_app = true;
            }

            // Aquí va código para detectar e interpretar más comandos...
            //                
        }
        catch ( const std::system_error& e )
        {
            // El error EINTR no se debe a un error real sino a una señal que interrumpió una llamada al sistema.
            // La ignoramos para comprobar si el manejador de señal cambió 'quit_app' y, si no, volver a intentar la
            // lectura del mensajes.
            if (e.code().value() != EINTR) throw;
        }
    }

    stop_alarm();
    
    // Vamos a salir del programa...
    std::println( "Ha llegado orden de terminar ¡Adiós!" );

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
