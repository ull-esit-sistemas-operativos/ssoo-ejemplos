// filelock-server.c - Servidor del ejemplo del uso de bloqueos de archivos
//
//  El programa servidor utiliza alarm() y las señales del sistema para mostrar periódicamente la hora. Además,
//  crea un archivo con el PID del proceso. El cliente puede usar este archivo para conocer el PID para enviar una
//  señal al servidor.
//
//  Esta técnica es muy usada por los servicios del sistema. Frecuentemente crean un subdirectorio con el nombre del
//  servicio dentro de /var/run. Allí colocan un archivo '.pid' con el PID del proceso, así como otros recursos
//  necesarios para la comunicación con el servicio, como sockets de dominio UNIX o FIFO.
//  El archivo '.pid' permite a los clientes saber si el servicio está en ejecución y mandarle señales para detenerlo
//  o reiniciarlo. 
//
//  Compilar:
//
//      gcc -I../ -o filelock-server filelock-server.c
//

#define _XOPEN_SOURCE   500

#include <unistd.h>     // Cabecera principal de la API POSIX del sistema operativo
#include <fcntl.h>      // Algunas operaciones del estándar POSIX con descriptores de archivo
                        // no están en <unistd.h> sino aquí. Por ejemplo open()
#include <signal.h>     // Funciones relacionadas con señales.
#include <limits.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "filelock-server.h"

const int ALARM_DEFAULT_TIME = 3 /* seg. */; 

int create_pidfile();
void cleanup_pidfile();

int read_pid_from_file (int fd, pid_t* pid);
int write_pid_to_file (int fd, pid_t pid);
bool check_if_process_exist( pid_t pid );

int main()
{
    int exit_code = 0;
    bool pidfile_created = false;

    // Las variables anteriores y las siguientes estructuras de if son para el manejo de errores en C. Así liberamos
    // recursos y dejamos todo en su sitio al terminar, incluso si es por un error. En C++ es mejor encapsular los
    // recursos en clases y usar su destructor para liberarlos (RAII).

    if (! exit_code)
    {
        exit_code = create_pidfile();
        if (exit_code == 0)
        {
            pidfile_created = true;
        }
    }

    if (! exit_code)
    {
        // Iniciar la alarma.
        alarm( ALARM_DEFAULT_TIME );

        // Configurar el set de señales por las que debe esperar sigwait().
        // sigwait() bloqueará el hilo hasta que llegue una de estas señales.
        sigset_t sigwaitset;
        sigemptyset( &sigwaitset );
        sigaddset ( &sigwaitset, SIGALRM);
        sigaddset ( &sigwaitset, SIGTERM);
        sigaddset ( &sigwaitset, SIGINT);

        // Bloquear la señales que por las que esperaremos en sigwait()
        sigprocmask( SIG_BLOCK, &sigwaitset, NULL );

        // Esperar hasta que una señal indique que el proceso debe terminar.
        while (true)
        {
            int signum;
            sigwait( &sigwaitset, &signum );

            if (signum == SIGALRM)
            {
                time_t current_time = time(NULL);
    
                // Como no necesitamos llamar a ctime() desde un manejador de señales ni desde varios hilos, no
                // necesitamos usar ctime_r(), que es la versión reentrante de ctime().
                char* current_time_string = ctime( &current_time );
                fputs( current_time_string, stdout );
         
                // Programar la siguiente alarma.
                alarm( ALARM_DEFAULT_TIME );
            }
            else if (signum == SIGTERM || signum == SIGINT)
            {
                // Terminar el proceso.
                break;
            }
        }

        // Detener la alarma.
        alarm(0);

        // Vamos a salir del programa...
        puts( "Ha llegado orden de terminar ¡Adiós!" );
    }

    if (pidfile_created)
    {
        cleanup_pidfile();
    }

    return exit_code;
}

int create_pidfile()
{
    int return_code = 0;
    bool file_opened = false;
    bool file_locked = false;
    int fd = -1;
    pid_t current_pid = -1;

    // Abrir el archivo con el PID para escritura.
    // Crearlo si no existe, con permisos 0600.
    if (! return_code)
    {
        fd = open( PID_FILENAME, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
        if (fd >= 0)
            file_opened = true;
        else
        {
            fprintf( stderr, "Error (%d) al abrir el archivo '%s': %s\n", errno, PID_FILENAME, strerror(errno) );
            return_code = 1;
        }
    }

    // Varios servidores pueden haberse lanzado al mismo tiempo. Solo uno debe poder escribir su PID en el archivo
    // y continuar ejecutándose. El resto deben detectar la situación y terminar inmediatamente.

    // Para evitar que se pisen unos a otros escribiendo su PID en el archivo, adquirimos el bloqueo.
    if (! return_code)
    {
        if (lockf( fd, F_LOCK, 0 ) == 0)
            file_locked = true;
        else
        {
            fprintf( stderr, "Error (%d) al bloquear el archivo PID: %s\n", errno, strerror(errno) );
            return_code = 2;
        }
    }    
    // A partir de este punto, el actual proceso tiene acceso exclusivo al archivo.

    // Puede que ya haya otro servidor en ejecución. Intentar leer su PID.
    if (! return_code)
    {
        return_code = read_pid_from_file( fd, &current_pid );
    }

    // Si se pudo leer el PID, comprobar si el proceso aun se está ejecutando.
    // Quizás murió sin tener tiempo de borrar el archivo.
    if (! return_code && current_pid > 0)
    {
        if (check_if_process_exist( current_pid ))
        {
            fputs( "Error: Hay otro servidor en ejecución.\n", stderr );
            return_code = 3;
        }
    }

    // Si el archivo esta vacío o el proceso ya no existe, escribir el PID del proceso actual. 
    // Mientras el proceso actual siga en ejecución, otros lo detectarán y terminarán prematuramente.
    if (! return_code)
    {
        return_code = write_pid_to_file( fd, getpid() );
    }

    // Desbloquear el acceso al archivo y cerrar el descriptor.
    if (file_locked) lockf( fd, F_ULOCK, 0 );
    if (file_opened) close( fd );

    return return_code;
}

int read_pid_from_file (int fd, pid_t* pid)
{
    char buffer[20];
    int bytes_read = read( fd, buffer, sizeof(buffer) );
    if (bytes_read < 0)
    {
        fprintf( stderr, "Error (%d) al leer el archivo PID: %s\n", errno, strerror(errno) );
        return 4;
    }

    // Si el archivo está vacío o es muy grande, seguramente no contenga un PID válido
    if (bytes_read == 0 || bytes_read == sizeof(buffer))
    {
        *pid = -1;
        return 0;
    }

    buffer[bytes_read] = '\0';
    char* endptr;
    unsigned long pid_readed  = strtoul( buffer, &endptr, 10 );

    // Si la conversión da valores de PID inválidos o no consume todos los caracteres leídos,
    // seguramente no contenga un PID válido.
    if ( pid_readed == 0 || pid_readed >= INT_MAX  || *endptr != '\0' )
    {
        *pid = -1;
        return 0;
    }

    *pid = pid_readed;

    return 0;
}

int write_pid_to_file (int fd, pid_t pid)
{
    char buffer[20];
    int string_length = snprintf( buffer, sizeof(buffer), "%d", pid );
    
    int truncate_result = ftruncate( fd, 0 );
    int bytes_written = write( fd, buffer, string_length );
    if (truncate_result < 0 || bytes_written < 0)
    {
        fprintf( stderr, "Error (%d) al escribir en el archivo PID: %s\n", errno, strerror(errno) );
        return 5;
    }

    return 0;
}

bool check_if_process_exist( pid_t pid )
{
    char process_pathname[PATH_MAX];
    snprintf( process_pathname, sizeof(process_pathname), "/proc/%d", pid );

    return (access( process_pathname, F_OK ) == 0);
}

void cleanup_pidfile()
{
    // Eliminar el archivo con el PID.
    int return_code = unlink( PID_FILENAME );
    if (return_code < 0) {
        fprintf( stderr, "Error (%d) al borrar el archivo: %s\n", errno,
            strerror(errno) );
    }
}
