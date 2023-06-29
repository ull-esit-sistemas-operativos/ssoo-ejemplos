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
#include <sstream>
#include <thread>

#define FMT_HEADER_ONLY
#include <fmt/format.h> // Hasta que std::format (C++20) esté disponible

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

void print_thread_info(std::thread& thread)
{
    std::stringstream ss;
    ss << thread.get_id();
    fmt::print( "Hilo creado: {} (0x{:x})\n",
        ss.str(),
        reinterpret_cast<uintptr_t>(thread.native_handle())
    );
}

int main()
{
    increment_counter_thread_args thread_args = { .counter = 0 };

    // Crear algunos hilos independientes cada uno de los cuales ejecutará increment_counter()    
    std::thread thread1(increment_counter, std::ref(thread_args));
    print_thread_info(thread1);

    std::thread thread2(increment_counter, std::ref(thread_args));
    print_thread_info(thread2);

    // Esperar a que los hilos terminen antes de continuar.
    // Si salimos de main() sin esperar, el proceso terminará y todos los hilos morirán inmediatamente,
    // sin tener tiempo de terminar adecuadamente. 
    thread1.join();
    thread2.join(); 

    fmt::print( "Valor final del contador: {}\n", thread_args.counter );

    return EXIT_SUCCESS;
}
