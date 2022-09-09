// pthreads-sync-factorial.cpp - Ejemplo del uso de mutex en POSIX Threads
//
// El programa calcula el factorial del número indicado por el usuario. Se utilizan dos hilos para paralelizar
// los cálculos, aprovechando mejor las CPU con varios núcleos.
//
//  Compilar:
//
//      g++ -I../ -I../../lib -lfmtlib -pthread -o pthreads-sync-factorial pthreads-sync-factorial.cpp
//

#include <pthread.h>

#include <cerrno>
#include <cstring>
#include <functional>
#include <iostream>
#include <numeric>
#include <vector>

#include <fmt/core.h>   // Hasta que std::format (C++20) esté disponible

#include <common/bigint_factorial.hpp>

struct factorial_thread_results
{
    pthread_mutex_t mutex;
    std::vector<BigInt> partials;
};

struct factorial_thread_args
{
    BigInt number;
    BigInt lower_bound;
    factorial_thread_results* results;
};

void* factorial_thread (void* arg)
{
    std::cout << fmt::format( "Hilo creado: 0x{:x}\n", pthread_self() );

    factorial_thread_args* args = static_cast<factorial_thread_args*>(arg);
    auto result = calculate_factorial( args->number, args->lower_bound );

    // Bloquear el mutex y guardar el resultado
    pthread_mutex_lock( &args->results->mutex );
    args->results->partials.push_back( result );
    pthread_mutex_unlock( &args->results->mutex );

    return nullptr;
}

int main()
{
    auto number = get_user_input();

    int return_code = 0;
    pthread_t thread1, thread2;

    factorial_thread_results thread_results;
    pthread_mutex_init( &thread_results.mutex, nullptr);

    // Para calcular el N!, un hilo multiplica desde N a N/2 y el otro desde (N/2)-1 hasta 2
    factorial_thread_args thread1_args { number, number / 2, &thread_results };
    factorial_thread_args thread2_args { (number / 2) - 1, 2, &thread_results };
    
    return_code = pthread_create(
        &thread1,
        nullptr,
        factorial_thread,
        &thread1_args );

    if (return_code)
    {
        std::cerr << fmt::format( "Error ({}) al crear el hilo: {}\n", return_code, strerror(return_code) );
        return EXIT_FAILURE;
    }

    return_code = pthread_create( &thread2, nullptr, factorial_thread, &thread2_args );
    if (return_code)
    {
        std::cerr << fmt::format( "Error ({}) al crear el hilo: {}\n", return_code, strerror(return_code) );
        
        // Al terminar main() aquí, estaremos abortando la ejecución del primer hilo, si no ha terminado antes.
        // Este caso es muy sencillo, así que no importa. Pero no suele ser buena idea no dejar que los hilos tengan
        // oportunidad de terminar por si mismos.
        return EXIT_FAILURE;
    }

    // Esperar a que los hilos terminen antes de continuar.
    // Si salimos de main() sin esperar, el proceso terminará y todos los hilos morirán inmediatamente,
    // sin tener tiempo de terminar adecuadamente. 
    pthread_join( thread1, nullptr );
    pthread_join( thread2, nullptr ); 

    // Combinar ambos resultados parciales en el factorial final.
    auto result = std::reduce( thread_results.partials.begin(), thread_results.partials.end(),
        BigInt{1}, std::multiplies<BigInt>() );

    std::cout << fmt::format( "El factorial de {} es {}\n", number.to_string(), result.to_string() );

    pthread_mutex_destroy( &thread_results.mutex);

    return EXIT_SUCCESS;
}
