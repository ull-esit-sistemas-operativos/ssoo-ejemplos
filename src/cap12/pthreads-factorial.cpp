// pthreads-factorial.cpp - Ejemplo del uso de threads con POSIX Threads
//
// El programa calcula el factorial del número indicado por el usuario. Se utilizan dos hilos para paralelizar
// los cálculos, aprovechando mejor las CPU con varios núcleos.
//
//  Compilar:
//
//      g++ -I../ -I../../lib -o pthreads-factorial pthreads-factorial.cpp
//

#include <cerrno>
#include <cstring>
#include <print>

#include <pthread.h>

#include <common/bigint-factorial.hpp>

struct factorial_thread_args
{
    BigInt number;
    BigInt lower_bound;
    BigInt result;
};

void* factorial_thread (void* arg)
{
    std::string output_label = std::format( "HILO 0x{:x}", pthread_self() );
    std::println( "[{}] Hilo creado", output_label );

    factorial_thread_args* args = static_cast<factorial_thread_args*>(arg);
    args->result = calculate_factorial( args->number, args->lower_bound, output_label );

    std::println( "[{}] Terminando...", output_label );

    return &args->result;
}

int main()
{
    auto number = get_user_input( "HILO PRINCIPAL" );

    int return_code = 0;
    pthread_t thread1, thread2;

    // Para calcular el N!, un hilo multiplica desde N a N/2 y el otro desde (N/2)-1 hasta 2
    // Luego será necesario multiplicar ambos resultados parciales para obtener el resultado final.
    factorial_thread_args thread1_args { .number = number, .lower_bound = number / 2, .result = 0 };
    factorial_thread_args thread2_args { .number = (number / 2) - 1, .lower_bound = 2, .result = 0 };
    
    return_code = pthread_create(
        &thread1,
        nullptr,
        factorial_thread,
        &thread1_args );

    if (return_code)
    {
        std::println( stderr, "[HILO PRINCIPAL] Error ({}) al crear el hilo: {}",
            return_code, std::strerror(return_code) );
        return EXIT_FAILURE;
    }

    return_code = pthread_create( &thread2, nullptr,  factorial_thread, &thread2_args );
    if (return_code)
    {
        std::println( stderr, "[HILO PRINCIPAL] Error ({}) al crear el hilo: {}",
            return_code, std::strerror(return_code) );
        
        // Al terminar main() aquí, estaremos abortando la ejecución del primer hilo, si no ha terminado antes.
        // Este caso es muy sencillo, así que no importa. Pero no suele ser buena idea no dejar que los hilos tengan
        // oportunidad de terminar por si mismos.
        return EXIT_FAILURE;
    }

    // Esperar a que los hilos terminen antes de continuar.
    // Si salimos de main() sin esperar, el proceso terminará y todos los hilos morirán inmediatamente,
    // sin tener tiempo de terminar adecuadamente. 
    BigInt* thread1_result, *thread2_result;

    pthread_join( thread1, reinterpret_cast<void**>(&thread1_result) );
    pthread_join( thread2,
        reinterpret_cast<void**>(&thread2_result) ); 

    // Combinar ambos resultados parciales en el factorial final.
    auto result = *thread1_result * *thread2_result;

    std::println( "[HILO PRINCIPAL] El factorial de {} es {}", number.to_string(), result.to_string() );

    return EXIT_SUCCESS;
}
