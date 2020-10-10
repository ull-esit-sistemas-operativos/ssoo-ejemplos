// pthread.cpp - Ejemplo del uso de threads con POSIX pthread
//
//  Compilar:
//
//      g++ -I../ -I../../lib -lfmtlib -pthread -o pthread pthread.cpp
//

#include <pthread.h>

#include <cerrno>
#include <cstring>
#include <iostream>

#include <fmt/core.h>   // Hasta que std::format (C++20) esté disponible

#include <common/bigint_factorial.hpp>

struct factorial_thread_args
{
    BigInt number;
    BigInt lower_bound;
    BigInt result;
};

void* factorial_thread (void* arg)
{
    std::cout << fmt::format( "Hilo creado: 0x{:x}\n", pthread_self() );

    factorial_thread_args* args = static_cast<factorial_thread_args*>(arg);
    args->result = calculate_factorial( args->number, args->lower_bound );

    return &args->result;
}

int main()
{
    auto number = get_user_input();

    int return_code = 0;
    pthread_t thread1, thread2;
    factorial_thread_args thread1_args { number, number / 2, 0 };
    factorial_thread_args thread2_args { (number / 2) - 1, 2, 0 };
    
    return_code = pthread_create(
        &thread1,
        nullptr,
        factorial_thread,
        &thread1_args );

    if (return_code)
    {
        std::cerr << fmt::format( "Error ({}) al crear el hilo: {}\n", return_code, strerror(return_code) );
        return 1;
    }

    return_code = pthread_create( &thread2, nullptr,  factorial_thread, &thread2_args );
    if (return_code)
    {
        std::cerr << fmt::format( "Error ({}) al crear el hilo: {}\n", return_code, strerror(return_code) );
        
        // Al terminar main() aquí, estaremos abortando la ejecución del primer hilo, si no ha terminado antes.
        // Este caso es muy sencillo, así que no importa. Pero no suele ser buena idea no dejar que los hilos tengan
        // oportunidad de terminar por si mismos.
        return 2;
    }

    // Esperar a que los hilos terminen antes de continuar.
    // Si salimos de main() sin esperar, el proceso terminará y todos los hilos morirán inmediatamente,
    // sin tener tiempo de terminar adecuadamente. 
    BigInt* thread1_result, *thread2_result;

    pthread_join( thread1, reinterpret_cast<void**>(&thread1_result) );
    pthread_join( thread2,
        reinterpret_cast<void**>(&thread2_result) ); 

    auto result = *thread1_result * *thread2_result;

    std::cout << fmt::format( "El factorial de {} es {}\n", number.to_string(), result.to_string() );

    return 0;
}
