// filelock-common.hpp - Cabecera común del ejemplo del uso de bloqueos de archivos
//
//  Es la versión con la API de Windows de ../posix/filelock-common.hpp
//

#pragma once

#include <string>

using namespace std::literals;

inline const std::string PID_FILENAME = "filelock.pid"s;

// La versión de POSIX no necesita esto. Allí el programa de control usa el PID que lee del archivo para mandarle al
// servidor la señal SIGTERM, y las señales son parte de la API del sistema. En Windows no hay señales, así que hace
// falta algo por lo que el servidor pueda esperar y que el programa de control pueda activar: un evento con nombre.
inline const std::string QUIT_EVENT_NAME = "Local\\ssoo-class-filelock-quit"s;
