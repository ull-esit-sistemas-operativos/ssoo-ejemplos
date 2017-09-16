// softstack.cpp - Ejemplo para mostrar la pila de software del sistema
//                 operativo
//
//      g++ -o softstack softstack.cpp
//
//      uso:
//              # ltrace -S -C -n4 ./softstack
//

#include <cstdlib>
#include <fstream>
#include <iostream>

int main()
{
    char filename[] = "/tmp/softstack-fileXXXXXX";
    mkstemp(filename);

    std::ofstream ofs;
    std::cout << "antes de abrir el archivo...\n";
    ofs.open(filename, std::ofstream::out);
    std::cout << "después de abrir el archivo...\n";
    ofs.close();

    return 0;
}
