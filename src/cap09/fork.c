// fork.cpp - Ejemplo del uso de fork() para crear procesos
//
//  Compilar:
//
//      gcc -o fork fork.c
//

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>

int main()
{
    pid_t pid = getpid();

    // Crear un proceso hijo
    pid_t child = fork();

    if (child == 0)
    {                   
        // Aquí solo entra el proceso hijo
        puts( "[HIJO] ¡Soy el proceso hijo!" );
        printf( "[HIJO] El valor de mi variable 'child' es: %d\n", child );
        printf( "[HIJO] Este es mi PID: %d\n", getpid() );
        printf( "[HIJO] El valor de mi variable 'pid' es: %d\n", pid );
        printf( "[HIJO] El PID de mi padre es: %d\n", getppid() );
        
        puts( "[HIJO] Durmiendo 10 segundos..." );
        sleep(10);

        int status = 56;
        printf( "[HIJO] Salgo con %d ¡Adios!\n", status );
        return status;
    }
    else if (child > 0)
    {   
        // Aquí solo entra el proceso padre
        puts( "[PADRE] ¡Soy el proceso padre!" );
        printf( "[PADRE] El valor de mi variable 'child' es: %d\n", child );
        printf( "[PADRE] Este es mi PID: %d\n", getpid() );
        printf( "[PADRE] El valor de mi variable 'pid' es: %d\n", pid );
        printf( "[PADRE] El PID de mi padre es: %d\n", getppid() );

        puts( "[PADRE] Voy a esperar a que mi hijo termine..." );
        
        int status;
        wait( &status );
        printf( "[PADRE] El valor de salida de mi hijo fue: %d\n", WEXITSTATUS(status) );
        
        puts( "[PADRE] ¡Adios!" );
        return 0;
    }
    else {
        // Aquí solo entra el padre si no pudo crear el hijo
        fprintf( stderr, "Error (%d) al crear el procesos: %s\n", errno, strerror(errno) );
        return 1;
    }
}
