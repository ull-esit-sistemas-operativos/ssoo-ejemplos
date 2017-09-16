// fifo-server.cpp - Servidor del ejemplo del uso de tuberías con nombre para
//                   comunicar procesos
//
//      g++ -o fifo-server fifo-server.cpp

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>
#include <string>

int main()
{
    const std::string fifopath = "/tmp/ssoo-class-fifo-server";

    // Crear la tubería con nombre.
    //  Como no hay función en el estándar de C++, usamos directamente la función
    //  mkfifo() de la librería de sistema
    int result = mkfifo(fifopath.c_str(), 0777);
    if (result < 0 && errno != EEXIST) {
        std::cerr << "fallo en mkfifo(): " << std::strerror(errno) << "\n";
        return 3;
    }

    // Abrir la tubería usando su nombre, como un archivo convencional
    //  Internamente: fd = open(fifopath, O_RDONLY);
    std::ifstream ifs {fifopath, std::ifstream::in};
    if (ifs.fail()) {
        std::cerr << "fallo al abrir la tubería\n";
        return 4;
    }

    std::cout << "SERVIDOR: ¡Soy el proceso servidor!\n";
    std::cout << "SERVIDOR: Este es mi PID: " << getpid() << "\n";

    // Leer de la tubería hasta que el cliente cierre la conexión u ocurra algún error
    while(ifs.good()) {
        std::string buffer;
        // Leer una línea de la tubería
        //  Internamente: read(fd, buffer, sizeof(buffer));
        std::getline(ifs, buffer);
        std::cout << "SERVIDOR: El cliente ha dicho '"<< buffer << "'\n";
    }
    
    // Eliminar la tubería con nombre.
    result = unlink(fifopath.c_str());
    if (result < 0) {
        std::cerr << "fallo en unlink(): " <<  std::strerror(errno) << "\n";
        return 5;
    }

    return 0;
}
