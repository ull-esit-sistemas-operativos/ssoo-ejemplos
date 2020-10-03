// shared-memory-server.h - Cabecera común del ejemplo del uso de memoria compartida con nombre para comunicar procesos
//

#pragma once

#include <semaphore.h>

#define CONTROL_SHM_NAME "/ssoo-class-shm-server"
#define QUIT_COMMAND "QUIT"

#define MAX_COMMAND_SIZE 100

struct memory_content
{
    sem_t empty;
    sem_t ready;
    char command_buffer[MAX_COMMAND_SIZE];
};
