// threads-sync-factorial.cpp - Ejemplo del uso de mutex en C++
//
// El programa calcula el factorial del número indicado por el usuario. Se utilizan dos hilos para paralelizar
// los cálculos, aprovechando mejor las CPU con varios núcleos. El resultado parcial del cálculo que realiza
// cada hilo se guarda en un std::vector  compartido, por lo que se usan mecanismos de sincronización para que los
// hilos no puedan modificar el vector al mismo tiempo.
//
//  Compilar:
//
//      g++ -I../ -I../../lib -o threads-sync-factorial threads-sync-factorial.cpp
//

#include <iostream>
#include <functional>
#include <mutex>
#include <numeric>
#include <thread>
#include <vector>

#define FMT_HEADER_ONLY
#include <fmt/format.h> // Hasta que std::format (C++20) esté disponible
#include <fmt/std.h>

#include <common/bigint_factorial.hpp>

struct factorial_thread_results
{   
    std::mutex mutex;
    std::vector<BigInt> partials;
};

void factorial_thread (factorial_thread_results& results, BigInt number, BigInt lower_bound)
{
    auto result = calculate_factorial( number, lower_bound );

    // Bloquear el mutex y guardar el resultado
    std::lock_guard<std::mutex> lock( results.mutex );
    results.partials.push_back( result );
    // El mutex se desbloquea al destruirse 'lock'
}

int main()
{
    auto number = get_user_input();

    factorial_thread_results thread_results;

    // Para calcular el N!, un hilo multiplica desde N a N/2 y el otro desde (N/2)-1 hasta 2
    auto thread1_lower_bound = number / 2;
    auto thread2_number = thread1_lower_bound - 1;

    std::thread thread1(factorial_thread, std::ref(thread_results), number, thread1_lower_bound);
    fmt::print( "Hilo creado: {} (0x{:x})\n", thread1.get_id(), thread1.native_handle() );

    std::thread thread2(factorial_thread, std::ref(thread_results), thread2_number, 2);
    fmt::print( "Hilo creado: {} (0x{:x})\n", thread2.get_id(), thread2.native_handle() );

    // Esperar a que los hilos terminen antes de continuar.
    // Si salimos de main() sin esperar, el proceso terminará y todos los hilos morirán inmediatamente,
    // sin tener tiempo de terminar adecuadamente. 
    thread1.join();
    thread2.join(); 

    // Combinar ambos resultados parciales en el factorial final.
    auto result = std::reduce( thread_results.partials.begin(), thread_results.partials.end(),
        BigInt{1}, std::multiplies<BigInt>() );

    fmt::print( "El factorial de {} es {}\n", number.to_string(), result.to_string() );

    return EXIT_SUCCESS;
}
