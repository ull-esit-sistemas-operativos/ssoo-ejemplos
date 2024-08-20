// threads-factorial.cpp - Ejemplo de creación de threads en C++
//
// El programa calcula el factorial del número indicado por el usuario. Se utilizan dos hilos para paralelizar
// los cálculos, aprovechando mejor las CPU con varios núcleos.
//
//  Compilar:
//
//      g++ -I../ -I../../lib -o threads-factorial threads-factorial.cpp
//

#include <chrono>
#include <print>
#include <sstream>      // Requerido para la conversion de std::thread::id
#include <thread>

#include <common/bigint-factorial.hpp>

using namespace std::chrono_literals;

const auto TIMEOUT = 5s;
const auto THREAD_POLLING_INTERVAL = 100ms;

void factorial_thread (std::stop_token stoken, BigInt& result, BigInt number, BigInt lower_bound)
{
    std::string output_label = std::format( "HILO {}", std::this_thread::get_id() );
    
    result = cancellable_calculate_factorial( stoken, number, lower_bound, output_label );
    
    std::println( "[{}] Terminando...", output_label );
}

int main()
{
    auto number = get_user_input( "HILO PRINCIPAL" );

    // Para calcular el N!, un hilo multiplica desde N a N/2 y el otro desde (N/2)-1 hasta 2.
    // Luego será necesario multiplicar ambos resultados parciales para obtener el resultado final.
    auto thread1_lower_bound = number / 2;
    auto thread2_number = thread1_lower_bound - 1;
    
    BigInt thread1_result, thread2_result;

    std::jthread thread1(factorial_thread, std::ref(thread1_result), number, thread1_lower_bound);
    std::println( "[HILO PRINCIPAL] Hilo creado: {} (0x{:x})", thread1.get_id(), thread1.native_handle() );

    std::jthread thread2(factorial_thread, std::ref(thread2_result), thread2_number, 2);
    std::println( "[HILO PRINCIPAL] Hilo creado: {} (0x{:x})", thread2.get_id(), thread2.native_handle() );

    // Esperar a que los hilos terminen antes de continuar contando el tiempo.
    // Si se supera TIMEOUT sin que los hilos hayan terminado, se cancelan los hilos y termina el programa.
    auto start = std::chrono::steady_clock::now();
    while (thread1_result == 0 || thread2_result == 0)
    {
        std::this_thread::sleep_for(THREAD_POLLING_INTERVAL);
        if (std::chrono::steady_clock::now() - start > TIMEOUT)
        {
            std::println( "[HILO PRINCIPAL] ¡Tiempo excedido! Cancelando..." );

            // Cancelar los hilos.
            thread1.request_stop();
            thread2.request_stop();

            // Esperar a que los hilos terminen.
            thread1.join();
            thread2.join(); 

            // Los hilos jthread se cancelan automáticamente al destruirse, por lo que el uso de
            // request_stop() y join() antes del return es opcional.
            return EXIT_FAILURE;
        }
    } 

    // Por la condición del 'while', en este punto sabemos que ambos hilos ya han terminado.
    // Por eso no necesitamos llamar a join() explícitamente.
    // thread1.join();
    // thread2.join(); 

    // Combinar ambos resultados parciales en el factorial final.
    auto result = thread1_result * thread2_result;

    std::println( "[HILO PRINCIPAL] El factorial de {} es {}", number.to_string(), result.to_string() );

    return EXIT_SUCCESS;
}
