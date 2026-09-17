// socket-common.hpp - Cabecera común del ejemplo del uso de sockets para comunicar procesos
//

#pragma once

#include <string>

using namespace std::literals;

inline const std::string CONTROL_SOCKET_NAME = "/tmp/ssoo-class-socket-server"s;
inline const std::string QUIT_COMMAND = "QUIT"s;
