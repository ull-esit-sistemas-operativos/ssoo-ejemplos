// pthreads-sync-counter.cpp - Ejemplo del uso de mutex en POSIX Threads
//
// El programa incrementa un contador usando varios hilos al mismo tiempo .
//
//  Compilar:
//
//      g++ -I../ -I../../lib -lfmtlib -o pthreads-sync pthreads-sync.cpp
//

#include <pthread.h>

#include <cerrno>
#include <cstring>
#include <iostream>

#include <fmt/core.h>   // Hasta que std::format (C++20) esté disponible

struct increment_counter_thread_args
{
    int counter;
    pthread_mutex_t mutex;
};

void* increment_counter (void* arg)
{
    std::cout << fmt::format( "Hilo creado: 0x{:x}\n", pthread_self() );

    increment_counter_thread_args* args = static_cast<increment_counter_thread_args*>(arg);

    // pthread_mutex_lock( &args->mutex );
    for (int i = 0; i < 1000000; i++) {
        // Bloquear el mutex antes de incrementar el contador.
        pthread_mutex_lock( &args->mutex );
        args->counter++;
        // Desbloquear el mutex tras incrementar el contador.
        pthread_mutex_unlock( &args->mutex );
    }
    // pthread_mutex_unlock( &args->mutex );

    std::cout << fmt::format( "Valor del contador: {}\n", args->counter );

    return nullptr;
}

int main()
{
    int return_code = 0;
    pthread_t thread1, thread2;

    increment_counter_thread_args thread_args = { 0 };
    
    // Inicializar el mutex en 'increment_counter_thread_args'
    pthread_mutex_init( &thread_args.mutex, nullptr);

    // Crear algunos hilos independientes cada uno de los cuales ejecutará increment_counter()
    return_code = pthread_create(
        &thread1,
        nullptr,
        increment_counter,
        &thread_args );

    if (return_code)
    {
        std::cerr << fmt::format( "Error ({}) al crear el hilo: {}\n", return_code, strerror(return_code) );
        return EXIT_FAILURE;
    }

    return_code = pthread_create( &thread2, nullptr, increment_counter, &thread_args );
    if (return_code)
    {
        std::cerr << fmt::format( "Error ({}) al crear el hilo: {}\n", return_code, strerror(return_code) );
        
        // Al terminar main() aquí, estaremos abortando la ejecución del primer hilo, si no ha terminado antes.
        // Este caso es muy sencillo, así que no importa. Pero no suele ser buena idea no dejar que los hilos tengan
        // oportunidad de terminar por sí mismos.
        return EXIT_FAILURE;
    }

    // Esperar a que los hilos terminen antes de continuar.
    // Si salimos de main() sin esperar, el proceso terminará y todos los hilos morirán inmediatamente,
    // sin tener tiempo de terminar adecuadamente. 
    pthread_join( thread1, nullptr );
    pthread_join( thread2, nullptr ); 

    std::cout << fmt::format( "Valor final del contador: {}\n", thread_args.counter );

    // Destruir el mutex cuando ya no es necesario.
    pthread_mutex_destroy( &thread_args.mutex );

    return EXIT_SUCCESS;
}
