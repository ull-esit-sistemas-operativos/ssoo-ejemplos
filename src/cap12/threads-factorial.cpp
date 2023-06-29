// threads-factorial.cpp - Ejemplo de creación de threads en C++
//
// El programa calcula el factorial del número indicado por el usuario. Se utilizan dos hilos para paralelizar
// los cálculos, aprovechando mejor las CPU con varios núcleos.
//
//  Compilar:
//
//      g++ -I../ -I../../lib -o threads-factorial threads-factorial.cpp
//

#include <iostream>
#include <thread>

#define FMT_HEADER_ONLY
#include <fmt/format.h> // Hasta que std::format (C++20) esté disponible
#include <fmt/std.h>

#include <common/bigint_factorial.hpp>

void factorial_thread (BigInt& result, BigInt number, BigInt lower_bound)
{
    result = calculate_factorial( number, lower_bound );
}

int main()
{
    auto number = get_user_input();

    BigInt thread1_result, thread2_result;

    // Para calcular el N!, un hilo multiplica desde N a N/2 y el otro desde (N/2)-1 hasta 2.
    auto thread1_lower_bound = number / 2;
    auto thread2_number = thread1_lower_bound - 1;

    std::thread thread1(factorial_thread, std::ref(thread1_result), number, thread1_lower_bound);
    fmt::print( "Hilo creado: {} (0x{:x})\n", thread1.get_id(), thread1.native_handle() );

    std::thread thread2(factorial_thread, std::ref(thread2_result), thread2_number, 2);
    fmt::print( "Hilo creado: {} (0x{:x})\n", thread2.get_id(), thread2.native_handle() );

    // Esperar a que los hilos terminen antes de continuar.
    // Si salimos de main() sin esperar, el proceso terminará y todos los hilos morirán inmediatamente,
    // sin tener tiempo de terminar adecuadamente. 
    thread1.join();
    thread2.join(); 

    // Combinar ambos resultados parciales en el factorial final.
    auto result = thread1_result * thread2_result;

    fmt::print( "El factorial de {} es {}\n", number.to_string(), result.to_string() );

    return EXIT_SUCCESS;
}
