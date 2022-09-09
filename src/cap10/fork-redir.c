// fork-redir.c - Ejemplo del uso de tuberías para redirigir la E/S estándar
//
//  Compilar:
//
//      gcc -o fork-redir fork-redir.c
//

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    // Crear una tubería
    int fds[2];
    int return_code = pipe( fds );
    if (return_code < 0) {
        fprintf( stderr, "Error (%d) al crear la tubería: %s\n", errno, strerror(errno) );
        return EXIT_FAILURE;
    }

    // Crear un proceso hijo
    pid_t child = fork();

    if (child == 0)
    {                   
        // Aquí solo entra el proceso hijo

        // El hijo hereda el acceso a la tubería, pero de su copia de los descriptores de los dos extremos solo
        // necesita el de escritura, para escribir la salida estándar del proceso.
        // Cerramos el de lectura.
        close( fds[0] );

        // Un programa, como el que vamos a ejecutar, que quiere mostrar algo por pantalla, generalmente usa la salida
        // estándar. La salida estándar está disponible en el descriptor de archivo STDOUT_FILENO (1), así que las
        // funciones printf() o puts() que usan los programas lo que realmente hacen es write(STDOUT_FILENO, texto, ...)
        //
        // Si queremos que lo que los programas escriban en su descriptor STDOUT_FILENO realmente vaya a la entrada
        // de la tubería, lo que se puede hacer es copiar las propiedades del descriptor en fds[1] (que es el de la
        // entrada de la tubería) en el descriptor STDOUT_FILENO. Así STDOUT_FILENO ya no sirve para mandar cosas a
        // donde las mandaba en el padre (la terminal, por ejemplo) sino a donde también las manda fds[1].
        //
        // Eso es lo que hace la siguiente función.
        dup2( fds[1], STDOUT_FILENO);

        // ...y una vez hecha la copia, ya no necesitamos el descriptor en fds[1]
        close( fds[1] );
        
        // Con todo configurado, sustituimos el programa actual por el que nos interesa.
        execl( "/bin/ls", "ls", "-l", NULL );
        
        fprintf( stderr, "Error (%d) al ejecutar el programa: %s\n", errno, strerror(errno) );
        return EXIT_FAILURE;
    }
    else if (child > 0)
    {   
        // Aquí solo entra el proceso padre 
        
        // El padre tiene acceso total a la tubería pero solo necesita el descriptor de lectura para obtener el
        // resultado del hijo. Además, el descriptor de escritura del hijo debe ser el único descriptor de escritura
        // abierto, para recibir un fin de archivo cuando el hijo muera.
        //
        // Cerramos el descriptor de escritura.
        close(fds[1]);
        
        // read() lee los bytes disponibles en la tubería. Para leer todo hay que leer hasta que devuelve 0, lo que
        // indica un fin de archivo. Es decir, que todos los descriptores de escritura están cerrados.
        char read_buffer[4096];
        char* read_buffer_begin = read_buffer;
        size_t total_bytes_read = 0;
        ssize_t bytes_read = 1;

        while (bytes_read > 0 && total_bytes_read < sizeof(read_buffer))
        {
            int bytes_left = sizeof(read_buffer) - total_bytes_read;
            bytes_read = read( fds[0], read_buffer_begin, bytes_left );
            if (bytes_read > 0)
            {
                read_buffer_begin += bytes_read;
                total_bytes_read += bytes_read;
            }
        }

        // Hemos leído hasta el final. Ya no necesitamos el descriptor de lectura.
        close( fds[0] );

        // Aquí solo se llega si read() devuelve 0, que indica fin de archivo, o -1, que es un error.  
        if (bytes_read < 0)
        {
            fprintf( stderr, "Error {%d} al leer la tubería: %s\n", errno, strerror(errno) );
            return EXIT_FAILURE;
        }

        // Sabemos que el hijo ha terminado porque el otro extremo de la tubería se cerró.
        // Aun así hay que llamar a wait() para:
        //
        //  a) Evitar que el proceso hijo se quede como proceso zombi.
        //  b) Obtener el estado de salida para saber si terminó con éxito (salió con 0) y, por tanto, que el
        //     contenido de buffer es válido.
        int status;
        wait( &status );

        if (! (WIFEXITED(status) && WEXITSTATUS(status) == 0) )
        {
            fputs( "Error: La tarea terminó inesperadamente.\n", stderr );
            return EXIT_FAILURE;
        }

        int num_of_lines = 0;
        for (const char* p = read_buffer; p < (read_buffer + total_bytes_read); ++p)
        {
            if (*p == '\n') ++num_of_lines;
        }

        printf( "La salida de 'ls' tiene %d líneas\n", num_of_lines );

        return EXIT_SUCCESS;
    }
    else {
        // Aquí solo entra el padre si no pudo crear el hijo
        fprintf( stderr, "Error (%d) al crear el proceso: %s\n", errno, strerror(errno) );

        close( fds[0] );
        close( fds[1] );
        return EXIT_FAILURE;
    }
}
