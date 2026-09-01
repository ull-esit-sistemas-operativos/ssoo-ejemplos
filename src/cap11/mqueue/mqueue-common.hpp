// mqueue-common.hpp - Cabecera común del ejemplo del uso de colas de mensajes para comunicar procesos
//

#pragma once

#include <string>

using namespace std::literals;

inline const std::string CONTROL_QUEUE_NAME = "/ssoo-class-queue-server"s;
inline const std::string QUIT_COMMAND = "QUIT"s;
