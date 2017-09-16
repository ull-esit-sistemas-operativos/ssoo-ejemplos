// shared-memory-sync.cpp - Ejemplo del uso de semáforos con memoria compartida
//
//      g++ -o shared-memory-sync shared-memory-sync.cpp -lpthread

#include <semaphore.h>
#include <unistd.h>
#include <sys/wait.h>

#include <cerrno>
#include <cstring>
#include <iostream>

#include "memory_map.h"

//
// Estructura de la memoria compartida
//
struct shared_counter
{
    sem_t semaphore;        // semáforo que proteje el acceso a la estructura
    int value;              // valor del contador
    
    shared_counter() : value{0}
    {
        // inicializar el semáforo
        int result = sem_init(&semaphore, 1, 1);
        if (result < 0) {
            throw std::system_error(errno, std::system_category(), "fallo en sem_init()");
        }
    }
    
    void lock()
    {
        sem_wait(&semaphore);
    }
    
    void unlock()
    {
        sem_post(&semaphore);
    }
};

int main()
{
    // pedir al sistema que asigne una zona de memoria compartida
    memory_map shared_mmap {PROT_READ | PROT_WRITE, sizeof(shared_counter)};

    // usar el new de emplazamiento para construir un objeto shared_counter,
    // sin reservar memoria, sobre la memoria compartida
    auto counter = new(shared_mmap.data<void*>()) shared_counter;

    // crear otro proceso
    pid_t pid = fork();

    if (pid == 0) {             // proceso hijo
        std::cout << "HIJO: ¡Soy el proceso hijo!\n";
        std::cout << "HIJO: Este es mi PID: " << getpid()<< "\n";
        std::cout << "HIJO: Voy a incrementar el contador 1000000 veces...\n";
        for (int i = 0; i < 1000000; i++) {
//            counter->lock();
            counter->value++;
//            counter->unlock();
        }

        std::cout << "HIJO: ¡Adios!\n";
        return 0;
    }
    else if (pid > 0) {         // proceso padre
        std::cout << "PADRE: ¡Soy el proceso padre!\n";
        std::cout << "PADRE: Este es mi PID: " << getpid()<< "\n";
        std::cout << "PADRE: Voy a incrementar el contador 1000000 veces...\n";
        for (int i = 0; i < 1000000; i++) {
//            counter->lock();
            counter->value++;
//            counter->unlock();
        }

        std::cout << "PADRE: Voy a esperar a que mi hijo termine...\n";
        int status;
        wait(&status);
        std::cout << "PADRE: Valor del contador: " << counter->value << "\n";
        std::cout << "PADRE: ¡Adios!\n";
        return 0;
    }
    else {                      // error
        std::cerr << "fallo en fork(): " << std::strerror(errno) << "\n";
        return 5;
    }
}
