// socket-common.hpp - Cabecera común del ejemplo del uso de sockets para comunicar procesos
//
//  La comparten las versiones para POSIX (posix/) y para la API de Windows (windows/) del ejemplo, porque la
//  dirección del socket de control y el formato de los comandos son los mismos en los dos sistemas.
//

#pragma once

#include <cstdint>
#include <string>

using namespace std::literals;

// El servidor escucha en la interfaz de loopback (127.0.0.1) y en este puerto UDP.
//
// Al usar la dirección de loopback solo pueden enviarle comandos los procesos del propio equipo. Si se usara
// INADDR_ANY, cualquiera en la red podría mandarle QUIT.
inline const std::uint16_t CONTROL_SOCKET_PORT = 12321;

inline const std::string QUIT_COMMAND = "QUIT"s;
