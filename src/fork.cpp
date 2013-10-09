// fork.cpp - Ejemplo del uso de fork() para crear procesos

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <cstdlib>
#include <iostream>

int main()
{
    // crear otro proceso
    pid_t pid = fork();

    if (pid == 0) {             // proceso hijo
        std::cout << "HIJO: ¡Soy el proceso hijo!" << std::endl;
        std::cout << "HIJO: Este es mi PID: " << getpid()<< std::endl;
        std::cout << "HIJO: El PID de mi padre es: " << getppid() << std::endl;
        std::cout << "HIJO: El valor de mi copia de 'pid' es: " << pid << std::endl;
        std::cout << "HIJO: Durmiendo 1 segundo..." << std::endl;
        sleep(1);
        std::cout << "HIJO: Voy a ejecutar el comando ls..." << std::endl;
        execlp("/bin/ls", "ls", "-l", NULL);
        std::cout << "HIJO: ¡Adios!" << std::endl;
        exit(1);
    }
    else if (pid > 0) {         // proceso padre
        int status;
        
        std::cout << "PADRE: ¡Soy el proceso padre!" << std::endl;
        std::cout << "PADRE: Este es mi PID: " << getpid()<< std::endl;
        std::cout << "PADRE: El PID de mi padre es: " << getppid() << std::endl;
        std::cout << "PADRE: El valor de mi copia de 'pid' es: " << pid << std::endl;
        std::cout << "PADRE: Voy a esperar a que mi hijo termine..." << std::endl;
        wait(&status);
        std::cout << "PARENT: El valor de salida de mi hijo fue: " <<
            WEXITSTATUS(status) << std::endl;
        std::cout << "PADRE: ¡Adios!" << std::endl;
        exit(2);
    }
    else {                      // error
        std::cerr << "fallo en fork()" << std::endl;
        exit(-1);
    }
}
