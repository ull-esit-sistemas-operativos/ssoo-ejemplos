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

#include <cstdlib>
#include <fstream>
#include <print>

int main()
{
    char filename[] = "/tmp/softstack-fileXXXXXX";

    std::ofstream ofs;
    mkstemp( filename );

    std::println( "Antes de abrir el archivo..." );
    ofs.open( filename, std::ofstream::out );
    std::println( "Después de abrir el archivo..." );
    ofs.close();

    return EXIT_SUCCESS;
}
