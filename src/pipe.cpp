// pipe.cpp - Ejemplo del uso de las tuberías para comunicar procesos
//
//      g++ -o pipe pipe.cpp

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <array>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <iostream>

int main()
{
    // crear una tubería
    std::array<int, 2> fds;
    int result = pipe(fds.data());
    if (result < 0) {
        std::cerr << "fallo en el pipe(): " << std::strerror(errno) << "\n";
        return 3;
    }
 
    pid_t pid = fork();
    if (pid == 0) {             // proceso hijo
        std::cout << "HIJO: ¡Soy el proceso hijo!\n";
        std::cout << "HIJO: Este es mi PID: " << getpid()<< "\n";
        close(fds[0]);
        write(fds[1], "El hijo ha sido creado", 23);
    }
    else if (pid > 0) {         // proceso padre
        close(fds[1]);
        
        std::array<char, 255> buffer;
        read(fds[0], buffer.data(), buffer.size());
        std::cout << "PADRE: ¡Soy el proceso padre!\n";
        std::cout << "PADRE: Este es mi PID: " << getpid()<< "\n";
        std::cout << "PADRE: Mi hijo ha dicho '" << buffer.data() << "'\n";

        // el padre esperará a que el hijo termine
        std::cout << "PADRE: Voy a esperar a que mi hijo termine...\n";
        wait(NULL);
        std::cout << "PADRE: ¡Adios!\n";
    }
    else {
        std::cerr << "fallo en fork(): " << std::strerror(errno) << "\n";
        return 4;
    }

    return 0;
}
