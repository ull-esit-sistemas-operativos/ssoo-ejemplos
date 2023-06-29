// threads-sync-counter.cpp - Ejemplo del uso de mutex en C++
//
// El programa incrementa un contador compartido usando varios hilos al mismo tiempo.
//
//  Compilar:
//
//      g++ -I../ -I../../lib -o threads-sync-counter threads-sync-counter.cpp
//

#include <iostream>
#include <mutex>
#include <thread>

#define FMT_HEADER_ONLY
#include <fmt/format.h> // Hasta que std::format (C++20) esté disponible
#include <fmt/std.h>

struct increment_counter_thread_args
{
    int counter;
    std::mutex mutex;
};

void increment_counter (increment_counter_thread_args& args)
{
    // args.mutex.lock();
    for (int i = 0; i < 1000000; i++) {
        // Bloquear el mutex antes de incrementar el contador.
        // args.mutex.lock();
        std::lock_guard<std::mutex> lock( args.mutex );
        args.counter++;
        // Desbloquear el mutex tras incrementar el contador.
        // args.mutex.unlock();
    }
    // args.mutex.unlock();

    fmt::print( "Valor del contador: {}\n", args.counter );
}

int main()
{
    increment_counter_thread_args thread_args = { .counter = 0 };

    // Crear algunos hilos independientes cada uno de los cuales ejecutará increment_counter()    
    std::thread thread1(increment_counter, std::ref(thread_args));
    fmt::print( "Hilo creado: {} (0x{:x})\n", thread1.get_id(), thread1.native_handle() );

    std::thread thread2(increment_counter, std::ref(thread_args));
    fmt::print( "Hilo creado: {} (0x{:x})\n", thread2.get_id(), thread2.native_handle() );

    // Esperar a que los hilos terminen antes de continuar.
    // Si salimos de main() sin esperar, el proceso terminará y todos los hilos morirán inmediatamente,
    // sin tener tiempo de terminar adecuadamente. 
    thread1.join();
    thread2.join(); 

    fmt::print( "Valor final del contador: {}\n", thread_args.counter );

    return EXIT_SUCCESS;
}
