// fifo.cpp - Ejemplo del uso de tuberías con nombre para comunicar procesos
//
//  El programa de ejemplo utiliza alarm() y las señales del sistema para mostrar periódicamente la hora. Además, crea
//  una tubería FIFO a la que puede conectarse el programa de control para darle órdenes.
//
//  Compilar:
//
//      g++ -I../ -o fifo fifo.cpp ../common/timeserver.cpp
//

#include <cerrno>       // La librería estándar de C está disponible tanto en cabeceras estilo <stdlib.h> como
#include <cstring>      // <cstdlib>. La primera es para usar con C, mientras que la segunda es la recomendada en C++
                        // pues mete las funciones en el espacio de nombres 'std', como el resto de la
                        // librería estándar de C++.
#include <print>
#include <string>
#include <system_error>

#include <unistd.h>     // Cabecera principal de la API POSIX del sistema operativo
#include <fcntl.h>      // Algunas operaciones del estándar POSIX con descriptores de archivo no están en <unistd.h>
                        // sino aquí. Por ejemplo open()
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#include <common/timeserver.hpp>
#include "fifo-common.hpp"

const size_t MAX_COMMAND_SIZE = 100;

int protected_main()
{
    // Como no hay función en el estándar de C ni en el de C++ para crear tuberías, usamos directamente mkfifo()
    // de la librería del sistema POSIX.
    if ( mkfifo( CONTROL_FIFO_PATH.c_str(), 0666 ) < 0)
    {
        throw std::system_error( errno, std::system_category(), "Fallo en mkfifo()" );
    }

    // Abrir la tubería, recientemente creada, usando su nombre como un archivo convencional.
    // Si abrimos en modo O_RDONLY, la operación se bloquea hasta que otro proceso abra la tubería en modo escritura.
    int controlfd = open( CONTROL_FIFO_PATH.c_str(), O_RDWR );
    if (controlfd < 0)
    {
        throw std::system_error( errno, std::system_category(), "Fallo en open()" );
    }

    std::println( "Escuchando en la tubería de control '{}'...", CONTROL_FIFO_PATH.c_str());

    // Teniendo un descriptor de archivos del sistema operativo se puede obtener un std::FILE* de la librería estándar
    // usando std::fdopen(). Así sería posible usar las facilidades de la librería estándar, para leer línea a línea.
    std::FILE* controlfile = fdopen( controlfd, "r" );

    // Comenzar a mostrar la hora periódicamente.
    start_alarm();
          
    // Leer de la tubería de control los comandos e interpretarlos.
    bool quit_app = false;
    while (!quit_app)
    {
        std::array<char, MAX_COMMAND_SIZE> buffer;
        
        // Leer de la tubería una línea con un comando. Incluye el '\n' final.
        char* command = std::fgets( buffer.data(), buffer.size(), controlfile );
        if ( command == nullptr ) break;

        // Eliminar el '\n' final.
        command[ std::strlen(command) - 1 ] = '\0';

        if ( QUIT_COMMAND == command )
        {
            std::println( "Ha llegado orden de terminar ¡Adiós!" );
            quit_app = true;
        }
        // Aquí va código para detectar e interpretar más comandos...
        //
        else
        {
            std::println( "Comando de control no reconocido: '{}'", command );
        }                
    }

    // Parar de mostrar la hora periódicamente.
    stop_alarm();

    // Cerrar la tubería de control.
    close(controlfd);
    // Eliminar la tubería con nombre. Nadie más podrá conectarse.
    unlink( CONTROL_FIFO_PATH.c_str() );

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
