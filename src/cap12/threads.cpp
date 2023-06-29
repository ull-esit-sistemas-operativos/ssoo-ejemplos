// threads.cpp - Ejemplo de creación de threads en C++
//
//  Compilar:
//
//      g++ -I../ -I../../lib -o threads threads.cpp
//

#include <iostream>
#include <thread>

#define FMT_HEADER_ONLY
#include <fmt/format.h> // Hasta que std::format (C++20) esté disponible
#include <fmt/std.h> 

void thread_function(int thread_id)
{
    fmt::print( "[Hilo {}] Creado\n", thread_id );
    for(int i = 0; i < 10; ++i)
    {
        // Dormir el hilo para simular que hace trabajo
        std::this_thread::sleep_for( std::chrono::microseconds(200) );
        fmt::print( "[Hilo {}] Ejecutada la iteración {}...\n", thread_id, i );
    }
    fmt::print( "[Hilo {}] Terminado\n", thread_id );
}

int main()
{
    // Crear 3 hilos dentro del proceso
    std::thread thread1( thread_function, 1 );
    std::thread thread2( thread_function, 2 );
    std::thread thread3( thread_function, 3 );

    std::cout << "[Main] Todos los hilos creados\n";

    fmt::print( "[Main] Hilo 1 - Id: {}, Manejador del sistema: 0x{:x}\n",
        thread1.get_id(), thread1.native_handle() );
    fmt::print( "[Main] Hilo 2 - Id: {}, Manejador del sistema: 0x{:x}\n",
        thread2.get_id(), thread2.native_handle() );
    fmt::print( "[Main] Hilo 3 - Id: {}, Manejador del sistema: 0x{:x}\n",
        thread3.get_id(), thread3.native_handle() );

    // Esperar a que los hilos terminen antes de continuar.
    // Si salimos de main() sin esperar, el proceso terminará y todos los hilos morirán inmediatamente,
    // sin tener tiempo de terminar adecuadamente. 
    thread1.join();
    thread2.join();
    thread3.join(); 

    return EXIT_SUCCESS;
}
