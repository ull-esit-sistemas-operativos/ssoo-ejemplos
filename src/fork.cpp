// fork.cpp - Ejemplo del uso de fork() para crear procesos

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <cerrno>
#include <cstring>
#include <iostream>

int main()
{
    // crear un proceso hijo
    pid_t pid = fork();

    if (pid == 0) {             // proceso hijo
        std::cout << "HIJO: ¡Soy el proceso hijo!\n";
        std::cout << "HIJO: Este es mi PID: " << getpid()<< "\n";
        std::cout << "HIJO: El PID de mi padre es: " << getppid() << "\n";
        std::cout << "HIJO: El valor de mi copia de 'pid' es: " << pid << "\n";
        std::cout << "HIJO: Durmiendo 1 segundo...\n";
        sleep(1);
        std::cout << "HIJO: Voy a ejecutar el comando ls...\n";
        execlp("/bin/ls", "ls", "-l", NULL);
        std::cout << "HIJO: ¡Adios!" << "\n";
        return 0;
    }
    else if (pid > 0) {         // proceso padre
        std::cout << "PADRE: ¡Soy el proceso padre!\n";
        std::cout << "PADRE: Este es mi PID: " << getpid() << "\n";
        std::cout << "PADRE: El PID de mi padre es: " << getppid() << "\n";
        std::cout << "PADRE: El valor de mi copia de 'pid' es: " << pid << "\n";
        std::cout << "PADRE: Voy a esperar a que mi hijo termine...\n";
        int status;
        wait(&status);
        std::cout << "PARENT: El valor de salida de mi hijo fue: " <<
            WEXITSTATUS(status) << "\n";
        std::cout << "PADRE: ¡Adios!\n";
        return 0;
    }
    else {                      // error
        std::cerr << "fallo en fork(): " << std::strerror(errno) << "\n";
        return 3;
    }
}
