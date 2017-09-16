// fifo-client.cpp - Cliente del ejemplo del uso de tuberías con nombre para
//                   comunicar procesos
//
//      g++ -o fifo-client fifo-client.cpp

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>
#include <string>

int main()
{
    const std::string fifopath = "/tmp/ssoo-class-fifo-server";

    // Abrir la tubería usando su nombre, como un archivo convencional
    //  Internamente: fd = open(fifopath, O_WRONLY);
    std::ofstream ofs {fifopath, std::ofstream::out};
    if (ofs.fail()) {
        std::cerr << "fallo al abrir la tubería\n";
        return 3;
    }

    std::cout << "CLIENTE: ¡Soy el proceso cliente!\n";
    std::cout << "CLIENTE: Este es mi PID: " << getpid() << "\n";

    while(std::cin.good()) {
        std::string line;
        std::getline(std::cin, line);
        // Enviar la línea leida de la entrada estándar por la tubería
        //  Internamente: write(fd, line.c_str(), line.size());
        ofs << line << std::endl;
    }

    return 0;
}
