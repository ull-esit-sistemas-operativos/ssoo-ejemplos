// softstack.cpp - Ejemplo en C++ para mostrar la pila de software del sistema operativo
//
//  Compilar:
//
//      g++ -o softstack-cpp softstack.cpp
//
//  Uso:
//
//      # strace -e trace=open,openat,read,write,close ./softstack-cpp
//

#include <cstdlib>
#include <fstream>
#include <iostream>

int main()
{
    char filename[] = "/tmp/softstack-fileXXXXXX";

    std::ofstream ofs;
    mkstemp( filename );

    std::cout << "Antes de abrir el archivo...\n";
    ofs.open( filename, std::ofstream::out );
    std::cout << "Después de abrir el archivo...\n";
    ofs.close();

    return EXIT_SUCCESS;
}
