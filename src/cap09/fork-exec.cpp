// fork-exec.cpp - Ejemplo del uso de fork() y exec() para ejecutar otros programas en C++
//
//  Compilar:
//
//      g++ -o fork-exec fork-exec.cpp
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
    // Crear un proceso hijo
    pid_t child = fork();

    if (child == 0)
    {                   
        // Aquí solo entra el proceso hijo
        std::println( "[HIJO] ¡Soy el proceso hijo!" );
        std::println( "[HIJO] Voy a ejecutar el comando 'ls'" );
        
        execl( "/bin/ls", "ls", "-l", nullptr );
        
        std::println( stderr, "Error ({}) al ejecutar el programa: {}", errno, std::strerror(errno) );
        return EXIT_FAILURE;
    }
    else if (child > 0)
    {   
        // Aquí solo entra el proceso padre
        std::println( "[PADRE] ¡Soy el proceso padre!" );
        std::println( "[PADRE] Voy a esperar a que mi hijo termine..." );
        
        int status;
        wait(&status);
        std::println( "[PADRE] El valor de salida de mi hijo fue: {}", WEXITSTATUS(status) );
        
        std::println( "[PADRE] ¡Adiós!" );
        return EXIT_SUCCESS;
    }
    else
    {
        // Aquí solo entra el padre si no pudo crear el hijo
        std::println( stderr, "Error ({}) al crear el proceso: {}", errno, std::strerror(errno));
        return EXIT_FAILURE;
    }
}
