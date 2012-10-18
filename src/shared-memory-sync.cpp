// shared-memory-sync.cpp - Ejemplo del uso de semáforos con memoria compartida
//
//      g++ -o shared-memory-sync shared-memory-sync.cpp -lpthread

#include <semaphore.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <cerrno>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <iostream>

//
// Estructura de la memoria compartida
//
struct Shared
{
    sem_t semaphore;            // semáforo que proteje el acceso a la estructura
    int counter;                // contador
};

int main()
{
    int result;
    Shared *shared;

    // pedir al sistema que asigne una zona de memoria compartida
    shared = (Shared*)mmap(NULL, sizeof(Shared), PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, -1, 0);
    if (shared == MAP_FAILED) { // error
        perror("fallo en mmap()");
        exit(-2);
    }

    // inicializar el semáforo de la memoria compartida
    result = sem_init(&shared->semaphore, 1, 1);
    if (result == -1) {
        perror("fallo en sem_init()");
        exit(-3);
    }

    // inicializar el contador
    shared->counter = 0;

    // crear otro proceso
    pid_t pid = fork();

    if (pid == 0) {             // proceso hijo
        std::cout << "HIJO: ¡Soy el proceso hijo!" << std::endl;
        std::cout << "HIJO: Este es mi PID: " << getpid()<< std::endl;
        std::cout << "HIJO: Voy a incrementar el contador 1000000 veces..." << std::endl;
        for (int i = 0; i < 1000000; i++) {
//            sem_wait(&shared->semaphore);
            shared->counter++;
//            sem_post(&shared->semaphore);
        }

        std::cout << "HIJO: ¡Adios!" << std::endl;
        exit(0);
    }
    else if (pid > 0) {         // proceso padre
        int status;

        std::cout << "PADRE: ¡Soy el proceso padre!" << std::endl;
        std::cout << "PADRE: Este es mi PID: " << getpid()<< std::endl;
        std::cout << "PADRE: Voy a incrementar el contador 1000000 veces..." << std::endl;
        for (int i = 0; i < 1000000; i++) {
//            sem_wait(&shared->semaphore);
            shared->counter++;
//            sem_post(&shared->semaphore);
        }

        std::cout << "PADRE: Voy a esperar a que mi hijo termine..." << std::endl;
        wait(&status);
        std::cout << "PADRE: Valor del contador: " << shared->counter << std::endl;
        std::cout << "PADRE: ¡Adios!" << std::endl;
        exit(0);
    }
    else {                      // error
        perror("fallo en fork()");
        exit(-1);
    }
}
