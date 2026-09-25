// namedpipe-common.hpp - Cabecera común del ejemplo del uso de tuberías con nombre para comunicar procesos
//
//  Es la versión con la API de Windows de ../posix/fifo-common.hpp
//

#pragma once

#include <string>

using namespace std::literals;

// Una tubería FIFO de POSIX es un archivo más del sistema de archivos, con su ruta y sus permisos, y se abre como
// cualquier otro. Las tuberías con nombre de la API de Windows no están en el sistema de archivos: viven en un
// espacio de nombres propio, al que se llega con rutas de la forma '\\.\pipe\<nombre>'. El '.' indica el equipo
// local; pero puede usarse el nombre de otro equipo, porque estas tuberías funcionan también a través de la red.
inline const std::string CONTROL_PIPE_NAME = "\\\\.\\pipe\\ssoo-class-fifo"s;

inline const std::string QUIT_COMMAND = "QUIT"s;
