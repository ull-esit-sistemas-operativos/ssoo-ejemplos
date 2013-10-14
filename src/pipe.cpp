// pipe.cpp - Ejemplo del uso de las tuberías para comunicar procesos
//
//      g++ -o pipe pipe.cpp

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <cstdlib>
#include <iostream>

int main()
{
    int fds[2];
    char buffer[255];

    // crear una tubería
    int res = pipe(fds);
    if (res < 0) {
        std::cerr << "fallo en el pipe()" << std::endl;
        exit(3);
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
        std::cout << "PADRO: Mi hijo ha dicho '"<< buffer << "'" << std::endl;

        // el padre esperará a que el hijo termine
        std::cout << "PADRE: Voy a esperar a que mi hijo termine..." << std::endl;
        wait(NULL);
        std::cout << "PADRE: ¡Adios!" << std::endl;
    }
    else {
        std::cerr << "fallo en fork()" << std::endl;
        exit(4);
    }

    exit(0);
}
