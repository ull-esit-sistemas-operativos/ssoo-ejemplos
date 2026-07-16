// fifo-server.h - Cabecera común del ejemplo del uso de tuberías con nombre para comunicar procesos
//

#pragma once

#include <string>

using namespace std::literals;

inline const std::string CONTROL_FIFO_PATH = "/tmp/ssoo-class-fifo"s;
inline const std::string QUIT_COMMAND = "QUIT"s;
