// fifo-control.cpp - Programa de control del ejemplo del uso de tuberías con nombre para comunicar procesos
//
//  El programa de ejemplo utiliza alarm() y las señales del sistema para mostrar periódicamente la hora. Además, crea
//  una tubería FIFO a la que puede conectarse el programa de control para darle órdenes.
//
//  Compilar:
//
//      g++ -o fifo-control-cpp fifo-control.cpp
//

#include <cerrno>       // La librería estándar de C está disponible tanto en cabeceras estilo <stdlib.h> como
#include <cstring>      // <cstdlib>. La primera es para usar con C, mientras que la segunda es la recomendada en C++
                        // pues mete las funciones en el espacio de nombres 'std', como el resto de la librería
                        // estándar de C++.
#include <fstream>
#include <print>
#include <string>

#include "fifo-common.hpp"

int main()
{
    // Abrir la tubería usando su nombre, como un archivo convencional.
    // También se puede abrir con open() o std::fopen().
    std::ofstream controlfs { CONTROL_FIFO_PATH, std::ifstream::out | std::ifstream::binary };
    if (controlfs.fail()) {
        std::println( stderr, "Error: No se pudo abrir la tubería.");
        return EXIT_FAILURE;
    }

    // Como los flujos de C++ son una abstracción de alto nivel de la librería estándar de C++, usándolos perdemos
    // algunas características, como poder conocer el motivo por el que no se puedo abrir el archivo. Así se podría
    // informar al usuario, por ejemplo, de que la tubería no existe así que, tal vez, el servidor no se está
    // ejecutando o de que no tiene los permisos adecuados. Si queremos más control, podemos usar open() y fdopen().

    std::println( "Cerrando el servidor..." );

    std::println( controlfs, "{}", QUIT_COMMAND );
    if (controlfs.bad()) {
        std::println( stderr, "Error: No se pudo escribir en la tubería.");
        return EXIT_FAILURE;
    }

    std::println( "¡Adiós!" );

    return EXIT_SUCCESS;
}
