// shared-memory-common.hpp - Cabecera común del ejemplo del uso de memoria compartida para comunicar procesos
//

#pragma once

#include <array>
#include <string>

#include <semaphore.h>

using namespace std::literals;

inline const std::string CONTROL_SHM_NAME = "/ssoo-class-shm"s;
inline const std::string QUIT_COMMAND = "QUIT"s;

struct memory_content
{
    sem_t empty;
    sem_t ready;
    std::array<char, 100> command_buffer;
    size_t command_length;
};
