// mmap.cpp - Ejemplo del uso de mmap() para mapear archivos

#include <algorithm>
#include <iostream>

#include "memory_map.h"

int main(int argc, char* argv[])
{
    if (argc != 3) {
        std::cout 
            << "uso: " << program_invocation_short_name << " <origen> <destino>\n";
        return 2;
    }
    
    memory_map src_mmap {argv[1], O_RDONLY};
    memory_map dst_mmap {argv[2], O_RDWR | O_CREAT, src_mmap.size()};

    // copiar el archivo de origen en el archivo de destino
    std::copy_n(src_mmap.data<uint8_t>(), src_mmap.size(), dst_mmap.data<uint8_t>());

    return 0;
}
