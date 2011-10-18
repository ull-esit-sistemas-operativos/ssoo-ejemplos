// softstack.cpp - Ejemplo para mostrar la pila de software del sistema
//                 operativo
//
//      g++ -o softstack softstack.cpp
//
//      uso:
//              # ltrace -S -C ./softstack
//                      o
//              # strace ./softstack

#include <cstdlib>
#include <fstream>
#include <iostream>

int main()
{
    char filename[] = "/tmp/softstack-fileXXXXXX";
    mkstemp(filename);

    std::ofstream ofs;
    std::cout << "antes de abrir el archivo..." << std::endl;
    ofs.open(filename);
    std::cout << "después de abrir el archivo..." << std::endl;
    ofs.close();

    return 0;
}
