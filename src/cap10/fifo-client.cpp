// fifo-client.cpp - Cliente del ejemplo del uso de tuberías con nombre para comunicar procesos
//
//  El programa servidor utiliza alarm() y las señales del sistema para mostrar periódicamente la hora. Además, crea
//  una tubería FIFO a la que puede conectarse el programa cliente para darle órdenes.
//
//  Compilar:
//
//      g++ -o fifo-client-cpp fifo-client.cpp
//

#include <cerrno>       // La librería estándar de C está disponible tanto en cabeceras estilo <stdlib.h> como
#include <cstring>      // <cstdlib>. La primera es para usar con C mientras que la segunda es la recomendada en C++
#include <iostream>     // pues mete las funciones en el espacio de nombres 'std', como el resto de la librería
#include <fstream>      // estándar de C++.
#include <string>

#include "fifo-server.h"

int main()
{
    // Abrir la tubería usando su nombre, como un archivo convencional.
    // También se puede abrir con open() o std::fopen().
    std::ofstream controlfs { CONTROL_FIFO_PATH, std::ifstream::out | std::ifstream::binary };
    if (controlfs.fail()) {
        std::cerr << "Error: No se pudo abrir la tubería.\n";
        return EXIT_FAILURE;
    }

    // Como los flujos de C++ son una abstracción de alto nivel de la librería estándar de C++, usándolos perdemos
    // algunas características, como poder conocer el motivo por el que no se puedo abrir el archivo. Así se podría
    // informar al usuario, por ejemplo, de que la tubería no existe así que, tal vez, el servidor no se está
    // ejecutando o de que no tiene los permisos adecuados. En el ejemplo 'file-client.c' se muestra  como usar 'errno'
    // con ese fin.

    std::cout << "Cerrando el servidor...\n";
    controlfs << QUIT_COMMAND << '\n';

    if (controlfs.bad()) {
        std::cerr << "Error: No se pudo escribir en la tubería.\n";
        return EXIT_FAILURE;
    }

    std::cout << "¡Adiós!\n";

    return EXIT_SUCCESS;
}
