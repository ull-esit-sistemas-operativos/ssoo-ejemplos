// threads.cpp - Ejemplo de creación de threads en C++
//
// El programa calcula el factorial del número indicado por el usuario. Se utilizan dos hilos para paralelizar
// los cálculos, aprovechando mejor las CPU con varios núcleos.
//
//  Compilar:
//
//      g++ -I../ -I../../lib -lfmtlib -pthread -o threads threads.cpp
//

#include <iostream>
#include <sstream>
#include <thread>

#include <fmt/core.h>   // Hasta que std::format (C++20) esté disponible

#include <common/bigint_factorial.hpp>

void factorial_thread (BigInt& result, BigInt number, BigInt lower_bound)
{
    std::stringstream ss;
    ss << std::this_thread::get_id();
    std::cout << fmt::format( "Hilo creado: {} (0x{:x})\n",
        ss.str(),
        pthread_self() );

    result = calculate_factorial( number, lower_bound );
}

int main()
{
    auto number = get_user_input();

    BigInt thread1_result, thread2_result;

    auto thread1_lower_bound = (number / 2) - 1;
    auto thread2_number = thread1_lower_bound - 1;

    std::thread thread1(factorial_thread, std::ref(thread1_result), number, thread1_lower_bound);
    std::thread thread2(factorial_thread, std::ref(thread2_result), thread2_number, 2);

    // Esperar a que los hilos terminen antes de continuar.
    // Si salimos de main() sin esperar, el proceso terminará y todos los hilos morirán inmediatamente,
    // sin tener tiempo de terminar adecuadamente. 
    thread1.join();
    thread2.join(); 

    auto result = thread1_result * thread2_result;

    std::cout << fmt::format( "El factorial de {} es {}\n", number.to_string(), result.to_string() );

    return 0;
}
