// pipe.cpp - Ejemplo del uso de las tuberías para comunicar procesos
//
//      g++ -o pipe pipe.cpp

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <cerrno>
#include <cstdlib>
#include <cstdio>
#include <iostream>

int main()
{
    int fds[2];
    char buffer[255];

    // crear una tubería
    int res = pipe(fds);
    if (res < 0) {
        perror("fallo en el pipe()");
        exit(-1);
    }
 
    pid_t pid = fork();
    if (pid == 0) {             // proceso hijo
        std::cout << "HIJO: ¡Soy el proceso hijo!" << std::endl;
        std::cout << "HIJO: Este es mi PID: " << getpid()<< std::endl;
        close(fds[0]);
        write(fds[1], "El hijo ha sido creado", 23);
    }
    else if (pid > 0) {         // proceso padre
        close(fds[1]);
        read(fds[0], buffer, sizeof(buffer));
        std::cout << "PADRE: ¡Soy el proceso padre!" << std::endl;
        std::cout << "PADRE: Este es mi PID: " << getpid()<< std::endl;
        std::cout << "PADRO: Mi hijo a dicho '"<< buffer << "'" << std::endl;

        // el padre esperará a que el hijo termine
        wait(NULL);
        std::cout << "PADRE: Voy a esperar a que mi hijo termine..." << std::endl;
        std::cout << "PADRE: ¡Adios!" << std::endl;
    }
    else {
        perror("fallo en fork()");
        exit(-2);
    }

    exit(0);
}
