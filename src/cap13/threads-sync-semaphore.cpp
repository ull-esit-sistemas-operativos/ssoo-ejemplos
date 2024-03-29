// threads-sync-semaphore.cpp - Ejemplo de uso de la clase `semaphore` implementada en C++
//
// El programa ejecuta varios hilos que acceden a una sección crítica sincronizada mediante un semáforo.
//
//  Compilar:
//
//      g++ -I../ -I../../lib -o threads-sync-semaphore threads-sync-semaphore.cpp
//

#include <iostream>
#include <ranges>
#include <vector>
#include <thread>

#define FMT_HEADER_ONLY
#include <fmt/format.h> // Hasta que std::format (C++20) esté disponible

#include "semaphore.hpp"

void thread_function(semaphore& sem, int thread_id)
{
    sem.acquire();
    fmt::print( "Hilo {} creado\n", thread_id );
    std::this_thread::sleep_for( std::chrono::seconds(2) ); // Dormir el hilo para simular trabajo
    fmt::print( "Hilo {} terminado\n", thread_id );
    sem.release();
}

int main()
{
    semaphore sem(3); // Solo permitir 3 hilos simultáneos

    std::vector<std::thread> threads;
    for(int i : std::views::iota(1, 10))
    {
        threads.push_back( std::thread( thread_function, std::ref(sem), i ));
    }

    for(auto& t : threads)
    {
        t.join();
    }

    return EXIT_SUCCESS;
}