// softstack.cpp - Ejemplo en C++ para mostrar la pila de software del sistema operativo
//
//  Compilar:
//
//      g++ -std=c++23 -o softstack-cpp softstack.cpp
//
//  Uso:
//
//      ltrace -CS -n4 ./softstack-cpp
//
//  En Windows no hay ltrace ni strace, pero se puede ver lo mismo con Process Monitor, que muestra las operaciones
//  sobre archivos, o con API Monitor, que muestra las llamadas a las funciones de las librerías del sistema.
//

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <print>

int main()
{
    auto filename = std::filesystem::temp_directory_path() / "softstack-file.txt";

    std::ofstream ofs;

    std::println( "Antes de abrir el archivo..." );
    ofs.open( filename, std::ofstream::out );
    std::println( "Después de abrir el archivo..." );
    ofs.close();

    std::filesystem::remove( filename );

    return EXIT_SUCCESS;
}
