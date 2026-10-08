// fork-redir.cpp - Ejemplo del uso de fork(), exec() y dup2() para redirigir la salida de otro programa a un archivo
//
//  Compilar:
//
//      g++ -std=c++23 -o fork-redir fork-redir.cpp
//

#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <cstdio>
#include <print>

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
    // Crear un proceso hijo
    pid_t child = fork();

    if (child < 0)
    {
        // Aquí solo entra el padre si no pudo crear el hijo
        std::println( stderr, "Error ({}) al crear el proceso: {}", errno, std::strerror(errno) );
        return EXIT_FAILURE;
    }
    else if (child == 0)
    {
        // Aquí solo entra el proceso hijo
        std::println( "[HIJO] Voy a ejecutar el comando 'ls' con la salida redirigida a 'salida.txt'" );

        // Vaciar el búfer de la salida estándar antes de redirigirla. Si la salida no es una terminal, el mensaje
        // anterior puede seguir en el búfer. Si no se vacía ahora, se perdería con exec() o, si llegara a vaciarse
        // después de dup2(), acabaría en el archivo.
        std::fflush( stdout );

        // Abrir el archivo en modo escritura, porque vamos a redirigir a él la salida estándar.
        // Si no existe, se crea y si existe, se trunca.
        int fd = open( "salida.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666 );
        if (fd < 0)
        {
            std::println( stderr, "[HIJO] Error ({}) al abrir el archivo: {}", errno, std::strerror(errno) );
            _exit( EXIT_FAILURE );
        }

        // Duplicar el descriptor del archivo sobre la salida estándar. Desde aquí, todo lo que se escriba en
        // STDOUT_FILENO (1) irá a parar al archivo, en lugar de a la terminal.
        if (dup2( fd, STDOUT_FILENO ) < 0)
        {
            std::println( stderr, "[HIJO] Error ({}) al redirigir la salida: {}", errno, std::strerror(errno) );
            _exit( EXIT_FAILURE );
        }

        // El archivo sigue abierto a través de STDOUT_FILENO, así que el descriptor original ya no es necesario.
        // Lo cerramos para que el programa que vamos a ejecutar no lo herede.
        close( fd );

        // El programa ejecutado hereda los descriptores de archivo abiertos, incluida la salida estándar
        // redirigida. 'ls' escribirá en el archivo sin saberlo.
        execl( "/bin/ls", "ls", "-l", nullptr );

        // Si llegamos aquí, hubo un error al ejecutar exec. La salida de error no está redirigida, así que el
        // mensaje se ve en la terminal.
        std::println( stderr, "[HIJO] Error ({}) al ejecutar el programa: {}", errno, std::strerror(errno) );
        _exit( 127 );
    }
    else
    {
        // Aquí solo entra el proceso padre. Su salida estándar no ha cambiado, porque dup2() solo afectó al hijo.
        std::println( "[PADRE] Voy a esperar a que mi hijo termine..." );

        int status;
        waitpid( child, &status, 0 );

        if (! (WIFEXITED(status) && WEXITSTATUS(status) == 0) )
        {
            std::println( "[PADRE] El comando 'ls' terminó inesperadamente." );
            return EXIT_FAILURE;
        }

        std::println( "[PADRE] La salida de 'ls' está en el archivo 'salida.txt'" );
        return EXIT_SUCCESS;
    }
}
