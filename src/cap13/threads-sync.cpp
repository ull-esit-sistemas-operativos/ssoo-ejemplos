// threads-sync.cpp - Ejemplo del uso de mutex en C++
//
// El programa calcula el factorial del número indicado por el usuario. Se utilizan dos hilos para paralelizar
// los cálculos, aprovechando mejor las CPU con varios núcleos.
//
//  Compilar:
//
//      g++ -I../ -I../../lib -lfmtlib -pthread -o threads-sync threads-sync.cpp
//

#include <iostream>
#include <functional>
#include <mutex>
#include <numeric>
#include <sstream>
#include <thread>
#include <vector>

#include <fmt/core.h>   // Hasta que std::format (C++20) esté disponible

#include <common/bigint_factorial.hpp>

struct factorial_thread_results
{   
    std::mutex mutex;
    std::vector<BigInt> partials;
};

void factorial_thread (factorial_thread_results& results, BigInt number, BigInt lower_bound)
{
    std::stringstream ss;
    ss << std::this_thread::get_id(),
    std::cout << fmt::format( "Hilo creado: {} (0x{:x})\n",
        ss.str(),
        pthread_self() );

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

    auto thread1_lower_bound = (number / 2) - 1;
    auto thread2_number = thread1_lower_bound - 1;

    std::thread thread1(factorial_thread, std::ref(thread_results), number, thread1_lower_bound);
    std::thread thread2(factorial_thread, std::ref(thread_results), thread2_number, 2);

    // Esperar a que los hilos terminen antes de continuar.
    // Si salimos de main() sin esperar, el proceso terminará y todos los hilos morirán inmediatamente,
    // sin tener tiempo de terminar adecuadamente. 
    thread1.join();
    thread2.join(); 

    auto result = std::reduce( thread_results.partials.begin(), thread_results.partials.end(),
        BigInt{1}, std::multiplies<BigInt>() );

    std::cout << fmt::format( "El factorial de {} es {}\n", number.to_string(), result.to_string() );

    return 0;
}
