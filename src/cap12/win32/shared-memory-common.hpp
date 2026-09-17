// shared-memory-common.hpp - Cabecera común del ejemplo del uso de memoria compartida para comunicar procesos
//
//  Es la versión con la API Win32 de ../posix/shared-memory-common.hpp
//

#pragma once

#include <array>
#include <string>

using namespace std::literals;

// Los objetos del sistema con nombre viven en un espacio de nombres propio. El prefijo 'Local\' los limita a la
// sesión del usuario actual, que es lo que interesa aquí; con 'Global\' serían visibles para todo el sistema, pero
// crearlos requiere privilegios.
inline const std::string CONTROL_SHM_NAME = "Local\\ssoo-class-shm"s;

// A diferencia de POSIX, los semáforos de Windows API no se pueden colocar dentro de la memoria compartida: son
// objetos del sistema y los procesos los comparten poniéndoles un nombre, igual que a la propia región de memoria.
inline const std::string EMPTY_SEMAPHORE_NAME = "Local\\ssoo-class-shm-empty"s;
inline const std::string READY_SEMAPHORE_NAME = "Local\\ssoo-class-shm-ready"s;

inline const std::string QUIT_COMMAND = "QUIT"s;

// Por eso esta estructura solo tiene los datos, mientras que la de POSIX lleva además los dos semáforos dentro.
struct memory_content
{
    std::array<char, 100> command_buffer;
    size_t command_length;
};
