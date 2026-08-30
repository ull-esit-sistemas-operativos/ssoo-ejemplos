// pthreads-cancel-factorial.cpp - Ejemplo de cancelación de hilos con POSIX Threads
//
// El programa calcula el factorial del número indicado por el usuario, repartiendo el trabajo entre dos hilos.
// Si el cálculo tarda más de TIMEOUT, el hilo principal cancela ambos hilos y termina.
//
// Es la versión con POSIX Threads de jthreads-factorial.cpp, que hace lo mismo con std::jthread y un token de
// cancelación.
//
//  Compilar:
//
//      g++ -I../ -I../../lib -o pthreads-cancel-factorial pthreads-cancel-factorial.cpp
//

#include <cerrno>
#include <chrono>
#include <cstring>
#include <print>
#include <thread>

#include <pthread.h>

#include <common/bigint-factorial.hpp>

using namespace std::chrono_literals;

const auto TIMEOUT = 5s;
const auto THREAD_POLLING_INTERVAL = 100ms;

struct factorial_thread_args
{
    BigInt number;
    BigInt lower_bound;
    BigInt result;             // Resultado final, solo si el hilo termina el cálculo
    BigInt partial_result;     // Último resultado parcial, por si el hilo es cancelado
};

// Versión cancelable de calculate_factorial() de <common/bigint-factorial.hpp>.
//
// El bucle no hace ninguna llamada al sistema, así que no tiene puntos de cancelación propios.
// Sin la llamada a pthread_testcancel() el hilo sería incancelable, por mucho que se le pida terminar.
BigInt cancellable_calculate_factorial(BigInt number, BigInt lower_bound, BigInt& partial_result,
    std::string_view output_label = "FACTORIAL")
{
    if (! output_label.empty())
    {
        std::print( "[{}] ", output_label);
    }

    std::println( "Calculando..." );

    lower_bound = lower_bound < 2 ? 2 : lower_bound;
    BigInt factorial = 1;
    for ( BigInt i = lower_bound; i <= number; i++ )
    {
        factorial = factorial * i;

        // Publicar el resultado parcial ANTES del punto de cancelación. Si el hilo se cancela, no ejecutará la
        // sentencia 'return' de más abajo, así que esto es lo único que el hilo principal podrá aprovechar.
        partial_result = factorial;

        pthread_testcancel();
    }

    return factorial;
}

void* factorial_thread (void* arg)
{
    std::string output_label = std::format( "HILO 0x{:x}", pthread_self() );
    std::println( "[{}] Hilo creado", output_label );

    factorial_thread_args* args = static_cast<factorial_thread_args*>(arg);
    args->result = cancellable_calculate_factorial(
        args->number, args->lower_bound, args->partial_result, output_label );

    std::println( "[{}] Terminando...", output_label );

    return &args->result;
}

int main()
{
    auto number = get_user_input( "HILO PRINCIPAL" );

    int return_code = 0;
    pthread_t thread1, thread2;

    // Para calcular el N!, un hilo multiplica desde N a N/2 y el otro desde (N/2)-1 hasta 2.
    // Luego será necesario multiplicar ambos resultados parciales para obtener el resultado final.
    factorial_thread_args thread1_args { .number = number, .lower_bound = number / 2,
        .result = 0, .partial_result = 0 };
    factorial_thread_args thread2_args { .number = (number / 2) - 1, .lower_bound = 2,
        .result = 0, .partial_result = 0 };

    return_code = pthread_create( &thread1, nullptr, factorial_thread, &thread1_args );
    if (return_code)
    {
        std::println( stderr, "[HILO PRINCIPAL] Error ({}) al crear el hilo: {}",
            return_code, std::strerror(return_code) );
        return EXIT_FAILURE;
    }

    return_code = pthread_create( &thread2, nullptr, factorial_thread, &thread2_args );
    if (return_code)
    {
        std::println( stderr, "[HILO PRINCIPAL] Error ({}) al crear el hilo: {}",
            return_code, std::strerror(return_code) );

        pthread_cancel( thread1 );
        pthread_join( thread1, nullptr );
        return EXIT_FAILURE;
    }

    // Esperar a que los hilos terminen antes de continuar contando el tiempo.
    // Si se supera TIMEOUT sin que los hilos hayan terminado, se cancelan los hilos y termina el programa.
    auto start = std::chrono::steady_clock::now();
    while (thread1_args.result == 0 || thread2_args.result == 0)
    {
        std::this_thread::sleep_for(THREAD_POLLING_INTERVAL);
        if (std::chrono::steady_clock::now() - start > TIMEOUT)
        {
            std::println( "[HILO PRINCIPAL] ¡Tiempo excedido! Cancelando..." );

            // Cancelar los hilos. Esto no es una petición que el hilo pueda atender ordenadamente
            // El hilo muere en el siguiente punto de cancelación sin, por ejemplo, llamar a destructores
            // de objetos locales.
            pthread_cancel( thread1 );
            pthread_cancel( thread2 );

            // Esperar a que los hilos terminen. Un hilo cancelado no devuelve el valor de su sentencia 'return',
            // sino PTHREAD_CANCELED, así que el resultado hay que leerlo de la estructura de argumentos.
            void *thread1_retval, *thread2_retval;

            pthread_join( thread1, &thread1_retval );
            pthread_join( thread2, &thread2_retval );

            std::println( "[HILO PRINCIPAL] Hilo 1 {}. Último resultado parcial: {}",
                thread1_retval == PTHREAD_CANCELED ? "cancelado" : "terminado",
                thread1_args.partial_result.to_string() );
            std::println( "[HILO PRINCIPAL] Hilo 2 {}. Último resultado parcial: {}",
                thread2_retval == PTHREAD_CANCELED ? "cancelado" : "terminado",
                thread2_args.partial_result.to_string() );

            return EXIT_FAILURE;
        }
    }

    // Por la condición del 'while', en este punto sabemos que ambos hilos ya han terminado.
    pthread_join( thread1, nullptr );
    pthread_join( thread2, nullptr );

    // Combinar ambos resultados parciales en el factorial final.
    auto result = thread1_args.result * thread2_args.result;

    std::println( "[HILO PRINCIPAL] El factorial de {} es {}", number.to_string(), result.to_string() );

    return EXIT_SUCCESS;
}
