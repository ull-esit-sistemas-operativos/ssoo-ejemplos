// fifo-server.c - Servidor del ejemplo del uso de tuberías con nombre para comunicar procesos
//
//  El programa servidor utilizar alarm() y las señales del sistema para mostrar periódicamente
//  la hora. Además, crea un tubería FIFO a la que puede conectarse el programa cliente para
//  darle órdenes.
//
//  Compilar:
//
//      gcc -o fifo-server fifo-server.c common.c
//

#define _POSIX_C_SOURCE 200809L

#include <unistd.h>     // Cabecera principal de la API POSIX del sistema operativo
#include <fcntl.h>      // Algunas operaciones del estándar POSIX con descriptores de archivo
                        // no están en <unistd.h> sino aquí. Por ejemplo open()
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "common.h"

const char* CONTROL_FIFO_PATH = "/tmp/ssoo-class-fifo-server";
const char* QUIT_COMMAND = "QUIT";

const size_t MAX_COMMAND_SIZE = 100;
const long CONTROL_POLLING_TIME = 500000000 /* ns. */;  // 500 ms.

int make_control_fifo();
void cleanup_control_fifo();

int open_control_fifo(int* controlfd);
int read_control_fifo(int controlfd, char* command);

int main()
{
    int return_value = 0;
    bool fifo_created = false;
    bool fifo_opened = false;
    int controlfd = -1;

    // Las variables anteriores y las siguientes estructuras de if son para el manejo de errores
    // en C. Asi liberarmos recursos y dejamos todo en su sitio al terminar, incluso si es por un
    // error. Por ejemplo, si al salir por un error olvidamos eliminar la tubería, no podríamos
    // volver a ejecutar el servidor hasta borrarla a mano. En C++ es mejor encapsular los recursos
    // en clases y usar su destructor para liberarlos (RAII).

    if (! return_value)
    {
        return_value = make_control_fifo();
        if (return_value == 0)
        {
            fifo_created = true;
        }
    }

    if (! return_value)
    {
        return_value = open_control_fifo(&controlfd);
        if (return_value == 0)
        {
            fifo_opened = true;
        }
    }

    if (! return_value)
    {
        printf( "Escuchando en la tubería de control '%s'...\n", CONTROL_FIFO_PATH);

        struct timespec polling_time =
        {
            .tv_sec = 0,
            .tv_nsec = CONTROL_POLLING_TIME
        };
          
        setup_signals();
        start_alarm();

        // Leer de la tubería de control los comandos e interpretarlos.
        while (!quit_app)
        {
            char command[MAX_COMMAND_SIZE + 1];

            return_value = read_control_fifo( controlfd, command );
            if (return_value != 0 || quit_app) break;

            if (command[0] == '\0')
            {
                // No hay nada que leer. Dormimos un rato el proceso para no quemar CPU.
                nanosleep(&polling_time, NULL);
            }
            else
            {
                if (strcmp(command, QUIT_COMMAND) == 0)
                {
                    quit_app = true;
                }

                // Aquí va código para detectar e interpretar más comandos...
                //                
            }
        }

        stop_alarm();
    }

    // Vamos a salir del programa...

    if (fifo_created)
    {
        cleanup_control_fifo();
    }

    if (fifo_opened)
    {
        close(controlfd);
    }

    return return_value;
}

int make_control_fifo()
{
    // Como no hay función en el estándar de C ni en el de C++ para crear tuberías usamos
    // directamente mkfifo() de la librería del sistema.
    int return_code = mkfifo( CONTROL_FIFO_PATH, 0777 );
    if (return_code < 0)
    {
        if (errno == EEXIST)
        {
            // Si ya existe la tubería, no deberíamos usarla porque habrían varios servidores
            // usando el mismo canal de control. Algunos mensajes llegarían a un servidor
            // y otros al otro. 
            fputs( "Error: Hay otro servidor en ejecución.\n", stderr );
            return 1;
        }
        else {
            fprintf( stderr, "Error (%d) al crear la tubería: %s\n", errno, strerror(errno) );
            return 2;
        }
    }
    
    return 0;
}

void cleanup_control_fifo()
{
    // Eliminar la tubería con nombre. Nadie más podrá conectarse.
    int return_code = unlink( CONTROL_FIFO_PATH );
    if (return_code < 0) {
        fprintf( stderr, "Error (%d) al borrar la tubería: %s\n", errno, strerror(errno) );
    }
}

int open_control_fifo(int* controlfd)
{
    // Abrir la tubería recientemente creada usando su nombre, como un archivo convencional.
    // También se puede abrir con std::fopen() o std::ifstream pero necesitamos pasar O_NONBLOCK
    // a open() porque sino se bloquea el proceso hasta que otro proceso abre la tubería para
    // lectura.
    *controlfd = open( CONTROL_FIFO_PATH, O_RDONLY | O_NONBLOCK );
    if (controlfd < 0)
    {
        fprintf( stderr, "Error (%d) al abrir la tubería: %s\n", errno, strerror(errno) );
        return 3;
    }

    // Teniendo un descriptor de archivos del sistemao operativo se puede obtener un FILE de la
    // librería estándar de C usando fdopen(). Así sería posible usar las facilidades de la
    // librería estándar. Como lo que vamos a hacer es sencillo, lo haremos trabajando
    // directamente con el descriptor de archivo y la librería del sistema.

    // Desactivar el modo O_NONBLOCK para que read() sea bloqueante.
    int fd_flags = fcntl( *controlfd, F_GETFL); 
    fcntl( *controlfd, F_SETFL, fd_flags & (~O_NONBLOCK) );

    return 0;
}

int read_control_fifo(int controlfd, char* command)
{
    // La dificultad está en que como se trata comunicacion orientada a flujos, no se conserva
    // la división entre mensajes (comandos). Tenemos que elegir un delimitador ('\n') al mandar
    // los mensajes y buscarlo al leer.

    char* buffer = command;
    char byte;
    size_t bytes_read = 0;

    while (true)
    {
        int return_code = read( controlfd, &byte, 1 );
        if (return_code < 0) 
        {
            if (errno == EINTR)
            {
                // Llegó una señal ¿debe terminar la aplicación o reintentar read()?
                if (quit_app)
                {
                    command[0] = '\0';
                    return 0;
                }
                else continue;
            }
            else { 
                fprintf( stderr, "Error (%d) al leer de la tubería: %s\n", errno, strerror(errno) );
                return 4;
            }
        }
        else if (return_code == 0)
        {
            // Ya no hay ningún proceso con el otro extremo abierto
            break;
        }
        else
        {
            if (byte == '\n')
            {
                break;
            }

            // Se leen los primeros MAX_COMMAND_SIZE de la linea.
            // El resto de caracteres se descartan.
            if (bytes_read < MAX_COMMAND_SIZE)
            {
                *buffer++ = byte;
                ++bytes_read;
            }
        }
    }

    *buffer = '\0';
    return 0;
}
