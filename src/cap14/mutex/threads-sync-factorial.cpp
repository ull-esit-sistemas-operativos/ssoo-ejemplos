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

#include <functional>
#include <mutex>
#include <numeric>
#include <print>
#include <sstream>      // Requerido para la conversion de std::thread::id
#include <thread>
#include <vector>

#include <common/bigint-factorial.hpp>

struct factorial_thread_results
{   
    std::mutex mutex;
    std::vector<BigInt> partials;
};

void factorial_thread (factorial_thread_results& results, BigInt number, BigInt lower_bound)
{
    std::string output_label = std::format( "HILO {}", std::this_thread::get_id() );
    auto result = calculate_factorial( number, lower_bound, output_label );

    // Bloquear el mutex y guardar el resultado
    std::lock_guard<std::mutex> lock( results.mutex );
    results.partials.push_back( result );
    // El mutex se desbloquea al destruirse 'lock'
}

int main()
{
    auto number = get_user_input( "HILO PRINCIPAL" );

    // Para calcular el N!, un hilo multiplica desde N a N/2 y el otro desde (N/2)-1 hasta 2
    // Luego será necesario multiplicar ambos resultados parciales para obtener el resultado final.
    auto thread1_lower_bound = number / 2;
    auto thread2_number = thread1_lower_bound - 1;
    
    factorial_thread_results thread_results;

    std::thread thread1(factorial_thread, std::ref(thread_results), number, thread1_lower_bound);
    std::println( "[HILO PRINCIPAL] Hilo creado: {} (0x{:x})", thread1.get_id(), thread1.native_handle() );

    std::thread thread2(factorial_thread, std::ref(thread_results), thread2_number, 2);
    std::println( "[HILO PRINCIPAL] Hilo creado: {} (0x{:x})", thread2.get_id(), thread2.native_handle() );

    // Esperar a que los hilos terminen antes de continuar.
    // Si salimos de main() sin esperar, el proceso terminará y todos los hilos morirán inmediatamente,
    // sin tener tiempo de terminar adecuadamente. 
    thread1.join();
    thread2.join(); 

    // Combinar ambos resultados parciales en el factorial final.
    auto result = std::reduce( thread_results.partials.begin(), thread_results.partials.end(),
        BigInt{1}, std::multiplies<BigInt>() );

    std::println( "[HILO PRINCIPAL] El factorial de {} es {}", number.to_string(), result.to_string() );

    return EXIT_SUCCESS;
}
