// fork-exec.c - Ejemplo del uso de fork() y exec() para ejecutar otros programas
//
//  Compilar:
//
//      gcc -o fork-exec fork-exec.c
//

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>

int main()
{
    // Crear un proceso hijo
    pid_t child = fork();

    if (child == 0)
    {                   
        // Aquí solo entra el proceso hijo
        puts( "[HIJO] ¡Soy el proceso hijo!" );
        puts( "[HIJO] Voy a ejecutar el comando 'ls'" );
        
        execl( "/bin/ls", "ls", "-l", NULL );
        
        fprintf( stderr, "Error (%d) al ejecutar el programa: %s\n", errno, strerror(errno) );
        return -1;
    }
    else if (child > 0)
    {   
        // Aquí solo entra el proceso padre
        puts( "[PADRE] ¡Soy el proceso padre!" );
        puts( "[PADRE] Voy a esperar a que mi hijo termine..." );
        
        int status;
        wait( &status );
        printf( "[PADRE] El valor de salida de mi hijo fue: %d\n", WEXITSTATUS(status) );
        
        puts( "[PADRE] ¡Adiós!" );
        return 0;
    }
    else {
        // Aquí solo entra el padre si no pudo crear el hijo
        fprintf( stderr, "Error (%d) al crear el proceso: %s\n", errno, strerror(errno) );
        return 3;
    }
}
