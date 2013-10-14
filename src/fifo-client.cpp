// fifo-client.cpp - Cliente del ejemplo del uso de tuberías con nombre para
//                   comunicar procesos
//
//      g++ -o fifo-client fifo-client.cpp

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>

int main()
{
    const char fifopath[] = "/tmp/ssoo-class-fifo-server";
    std::string line;

    // abrir la tubería con nombre
    // int fd = open(fifopath, O_WRONLY);
    std::ofstream ofs(fifopath);
    if (ofs.fail()) {
        std::cerr << "fallo al abrir la tubería" << std::endl;
        exit(3);
    }

    std::cout << "CLIENTE: ¡Soy el proceso cliente!" << std::endl;
    std::cout << "CLIENTE: Este es mi PID: " << getpid() << std::endl;

    while(std::cin.good()) {
        std::getline(std::cin, line);
        // write(fd, line.c_str(), line.size());
        ofs << line << std::endl;
    }

    exit(0);
}
