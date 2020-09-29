// fifo-client.cpp - Cliente del ejemplo del uso de tuberías con nombre para comunicar procesos
//
//  El programa servidor utilizar alarm() y las señales del sistema para mostrar periódicamente
//  la hora. Además, crea un tubería FIFO a la que puede conectarse el programa cliente para
//  darle órdenes.
//

#include <cerrno>       // La librería estándar de C está disponible tanto en cabeceras estilo 
#include <cstring>      // <stdlib.h> como <cstdlib>. La primera es para usar con C mientras que
#include <iostream>     // la segunda es la recomendada en C++ pues mete las funciones en el
#include <fstream>      // espacio de nombres 'std', como el resto de la librería estándar de C++.
#include <string>

#include <fmt/core.h>   // Hasta que std::format (C++20) esté disponible

using namespace std::string_literals;

const std::string CONTROL_FIFO_PATH = "/tmp/ssoo-class-fifo-server"s;
const std::string QUIT_COMMAND = "QUIT"s;

int main()
{
    // Abrir la tubería usando su nombre, como un archivo convencional.
    // También se puede abrir con open() o std::fopen().
    std::ofstream controlfs { CONTROL_FIFO_PATH, std::ifstream::out | std::ifstream::binary };
    if (controlfs.fail()) {
        std::cerr << "Error: No se pudo abrir la tubería.\n";
        return 1;
    }

    // Como los flujos de C++ son una abstracción de alto nivel de la librería estándar de C++,
    // usándolos perdemos algunas características, como poder conocer el motivo por el que no se
    // puedo abrir el archivo. Así se podría informar al usuario, por ejemplo, de que la tubería
    // no existe así que, tal vez, el servidor no se está ejecutando o de que no tiene los permisos
    // adecuados. En el ejemplo 'file-client.c' se muestra como usar 'errno' con ese fin.

    std::cout << "Cerrando el servidor...\n";
    controlfs << QUIT_COMMAND << '\n';

    if (controlfs.bad()) {
        std::cerr << "Error: No se pudo escribir en la tubería.\n";
        return 2;
    }

    std::cout << "¡Adios!\n";

    return 0;
}
