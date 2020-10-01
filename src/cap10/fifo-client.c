// fifo-client.c - Cliente del ejemplo del uso de tuberías con nombre para comunicar procesos
//
//  El programa servidor utilizar alarm() y las señales del sistema para mostrar periódicamente
//  la hora. Además, crea un tubería FIFO a la que puede conectarse el programa cliente para
//  darle órdenes.
//
//  Compilar:
//
//      gcc -o fifo-client-c fifo-client.c
//

#include <unistd.h>     // Cabecera principal de la API POSIX del sistema operativo
#include <fcntl.h>      // Algunas operaciones del estándar POSIX con descriptores de archivo
                        // no están en <unistd.h> sino aquí. Por ejemplo open()
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "fifo-server.h"

const char* make_quit_command()
{
    static const char buffer[] = QUIT_COMMAND"\n";
    return buffer;
}

int main()
{
    // Abrir la tubería usando su nombre, como un archivo convencional.
    // También se puede abrir con fopen() o, en C++, con std::ifstream.
    int controlfd = open( CONTROL_FIFO_PATH, O_WRONLY );
    if (controlfd < 0)
    {
        if (errno == EEXIST)
        {
            fputs( "Error: El servidor no parece estar en ejecución.\n", stderr);
            return 1;
        }
        else {
            fprintf( stderr, "Error (%d) al abrir la tubería: %s\n", errno, strerror(errno) );
            return 2;
        }
    }

    puts( "Cerrando el servidor..." );

    const char* command = make_quit_command();
    int return_code = write ( controlfd, command, strlen(command) );
    if (return_code < 0)
    {
        fprintf( stderr, "Error (%d) al escribir en la tubería: %s\n", errno, strerror(errno) );
        return 3;
    }

    puts( "¡Adios!" );
    close( controlfd );

    return 0;
}
