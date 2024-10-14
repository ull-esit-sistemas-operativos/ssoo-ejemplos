// fork.cpp - Ejemplo del uso de fork() para crear procesos en C++
//
//  Compilar:
//
//      g++ -o fork fork.cpp
//

#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <print>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
    pid_t pid = getpid();

    // Crear un proceso hijo
    pid_t child = fork();

    if (child == 0)
    {                   
        // Aquí solo entra el proceso hijo
        std::println( "[HIJO] ¡Soy el proceso hijo!" );
        std::println( "[HIJO] El valor de mi variable 'child' es: {}", child );
        std::println( "[HIJO] Este es mi PID: {}", getpid() );
        std::println( "[HIJO] El valor de mi variable 'pid' es: {}", pid );
        std::println( "[HIJO] El PID de mi padre es: {}", getppid() );
        
        std::println( "[HIJO] Durmiendo 10 segundos..." );
        sleep( 10 );

        int status = 42;
        std::println( "[HIJO] Salgo con {} ¡Adiós!", status );
        return status;
    }
    else if (child > 0)
    {   
        // Aquí solo entra el proceso padre
        std::println( "[PADRE] ¡Soy el proceso padre!" );
        std::println( "[PADRE] El valor de mi variable 'child' es: {}", child );
        std::println( "[PADRE] Este es mi PID: {}", getpid() );
        std::println( "[PADRE] El valor de mi variable 'pid' es: {}", pid );
        std::println( "[PADRE] El PID de mi padre es: {}", getppid() );

        std::println( "[PADRE] Voy a esperar a que mi hijo termine..." );
        
        int status;
        wait( &status );
        std::println( "[PADRE] El valor de salida de mi hijo fue: {}", WEXITSTATUS(status) );
        
        std::println( "[PADRE] ¡Adiós!" );
        return EXIT_SUCCESS;
    }
    else
    {
        // Aquí solo entra el padre si no pudo crear el hijo
        std::println( stderr, "Error ({}) al crear el proceso: {}", errno, strerror(errno) );
        return EXIT_FAILURE;
    }
}
