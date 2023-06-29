// anom-shared-memory.cpp - Ejemplo del uso de memoria compartida anónima para comunicar procesos
//
//  El programa solicita al usuario un número por la entrada estándar, lanza un proceso hijo para que calcule el
//  factorial y lee de la región de memoria comparida el resultado para utilizarlo.
//
//  Compilar:
//
//      g++ -I../ -o anom-shared-memory anom-shared-memory.cpp ../common/factorial.cpp
//

#include <unistd.h>     // Cabecera principal de la API POSIX del sistema operativo
#include <semaphore.h>  // Cabecera para los semáforos
#include <sys/mman.h>   // Cabecera para mmap()
#include <sys/types.h>
#include <sys/wait.h>

#include <iostream>
#include <cerrno>       // La librería estándar de C está disponible tanto en cabeceras estilo  <stdlib.h> como
#include <cstring>      // <cstdlib>. La primera es para usar con C mientras que la segunda es la recomendada en C++
                        // pues mete las funciones en el espacio de nombres 'std', como el resto de la librería
                        // estándar de C++.

#define FMT_HEADER_ONLY
#include <fmt/format.h> // Hasta que std::format (C++20) esté disponible

#include <common/factorial.hpp>

struct memory_content
{
    sem_t ready;        // Semáforo para indicar al padre cuándo está listo el resultado
    int factorial;      // Para guardar el resultado de calcular el factorial
};

int main()
{
    int exit_code = 0;

    // Reservar la zona de memoria compartida
    void* shared_mem = mmap(
        nullptr,
        sizeof(memory_content),
        PROT_READ | PROT_WRITE,
        MAP_ANONYMOUS | MAP_SHARED,
        -1,
        0 );

    if (shared_mem == MAP_FAILED) {
        fmt::print( stderr, "Error ({}) al reservar la memoria compartida: {}\n", errno, std::strerror(errno) );
        return EXIT_FAILURE;
    }

    memory_content* memory_region = static_cast<memory_content*>(shared_mem);

    // Necesitamos un mecanismo para sincronizar padre e hijo. El padre necesita esperar a que haya un resultado en
    // 'factorial' y el hijo necesita una forma de indicarlo.
    // Para esto el sistema ofrece mecanismos de sincronización, como los semáforos.
    //
    // Inicializar el semáforo.
    sem_init( &memory_region->ready, 1, 0);

    // Como el hijo tiene una copia de la memoria del padre antes del fork() no hace falta meter 'number' en la
    // memoria compartida.
    int number = get_user_input();

    // Crear el proceso hijo para el cálculo del factorial
    pid_t child = fork();
    if (child == 0)
    {   
        // Aquí solo entra el proceso hijo

        memory_region->factorial = calculate_factorial( number );

        // Indicar que ya está el resultado.
        sem_post( &memory_region->ready );

        // La memoria compartida se liberará al terminar el proceso, pero la liberamos a mano por tener un ejemplo
        // de cómo sería. 
        munmap( memory_region, sizeof(memory_content) );

        return EXIT_SUCCESS;
    }
    else if (child > 0)
    {
        // Aquí solo entra el proceso padre 
        
        // Poner el proceso a la espera de que esté el resultado.
        sem_wait( &memory_region->ready );
        
        fmt::print( "[PADRE] El factorial de {} es {}\n", number,
            memory_region->factorial );

        // Sabemos que el hijo ha terminado porque ya está el resultado. Aun así hay que llamar a wait() para evitar
        // que el proceso hijo se quede como proceso zombi.
        wait(nullptr);
    }
    else
    {
        // Aquí solo entra el padre si no pudo crear el hijo
        fmt::print( stderr, "Error ({}) al crear el proceso: {}\n", errno, strerror(errno) );
        exit_code = 4;
    }

    sem_destroy( &memory_region->ready );
    munmap( memory_region, sizeof(memory_content) );

    return exit_code;
}
