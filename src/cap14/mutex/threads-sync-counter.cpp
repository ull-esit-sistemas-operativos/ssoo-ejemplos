// threads-sync-counter.cpp - Ejemplo del uso de mutex en C++
//
// El programa incrementa un contador compartido usando varios hilos al mismo tiempo.
//
//  Compilar:
//
//      g++ -I../ -I../../lib -o threads-sync-counter threads-sync-counter.cpp
//

#include <mutex>
#include <print>
#include <sstream>      // Requerido para la conversion de std::thread::id
#include <thread>

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

    std::println( "Valor del contador: {}", args.counter );
}

int main()
{
    increment_counter_thread_args thread_args = { .counter = 0 };

    // Crear algunos hilos independientes cada uno de los cuales ejecutará increment_counter()    
    std::thread thread1(increment_counter, std::ref(thread_args));
    std::println( "Hilo creado: {} (0x{:x})", thread1.get_id(), thread1.native_handle() );

    std::thread thread2(increment_counter, std::ref(thread_args));
    std::println( "Hilo creado: {} (0x{:x})", thread2.get_id(), thread2.native_handle() );

    // Esperar a que los hilos terminen antes de continuar.
    // Si salimos de main() sin esperar, el proceso terminará y todos los hilos morirán inmediatamente,
    // sin tener tiempo de terminar adecuadamente. 
    thread1.join();
    thread2.join(); 

    std::println( "Valor final del contador: {}", thread_args.counter );

    return EXIT_SUCCESS;
}
