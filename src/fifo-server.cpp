// fifo-server.cpp - Servidor del ejemplo del uso de tuberías con nombre para
//                   comunicar procesos
//
//      g++ -o fifo-server fifo-server.cpp

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>

int main()
{
    const char fifopath[] = "/tmp/ssoo-class-fifo-server";
    std::string buffer;

    // crear la tubería con nombre
    int res = mkfifo(fifopath, 0777);
    if (res < 0 && errno != EEXIST) {
        std::cerr << "fallo en el mkfifo()" << std::endl;
        exit(3);
    }

    // abrir la tubería con nombre
    // int fd = open(fifopath, O_RDONLY);
    std::ifstream ifs(fifopath);
    if (ifs.fail()) {
        std::cerr << "fallo al abrir la tubería" << std::endl;
        exit(4);
    }

    std::cout << "SERVIDOR: ¡Soy el proceso servidor!" << std::endl;
    std::cout << "SERVIDOR: Este es mi PID: " << getpid() << std::endl;

    // leer de la tubería hasta que el cliente cierre la conexión
    // u ocurra algún error
    while(ifs.good()) {
        // read(fd, buffer, sizeof(buffer));
        ifs >> buffer;
        std::cout << "SERVIDOR: El cliente ha dicho '"<< buffer << "'" << std::endl;
    }

    exit(0);
}
