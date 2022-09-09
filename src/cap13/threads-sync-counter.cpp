// threads-sync.cpp - Ejemplo del uso de mutex en C++
//
// El programa incrementa un contador compartido usando varios hilos al mismo tiempo.
//
//  Compilar:
//
//      g++ -I../ -I../../lib -lfmtlib -pthread -o threads-sync-counter threads-sync-counter.cpp
//

#include <iostream>
#include <mutex>
#include <sstream>
#include <thread>

#include <fmt/core.h>   // Hasta que std::format (C++20) esté disponible

struct increment_counter_thread_args
{
    int counter;
    std::mutex mutex;
};

void increment_counter (increment_counter_thread_args& args)
{
    std::stringstream ss;
    ss << std::this_thread::get_id(),
    std::cout << fmt::format( "Hilo creado: {} (0x{:x})\n",
        ss.str(),
        pthread_self() );

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

    std::cout << fmt::format( "Valor del contador: {}\n", args.counter );
}

int main()
{
    increment_counter_thread_args thread_args = { 0 };

    // Crear algunos hilos independientes cada uno de los cuales ejecutará increment_counter()    
    std::thread thread1(increment_counter, std::ref(thread_args));
    std::thread thread2(increment_counter, std::ref(thread_args));

    // Esperar a que los hilos terminen antes de continuar.
    // Si salimos de main() sin esperar, el proceso terminará y todos los hilos morirán inmediatamente,
    // sin tener tiempo de terminar adecuadamente. 
    thread1.join();
    thread2.join(); 

    std::cout << fmt::format( "Valor final del contador: {}\n", thread_args.counter );

    return EXIT_SUCCESS;
}
