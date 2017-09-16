// threads.cpp - Ejemplo de creación de threads en C++
//
//      g++ -std=c++11 -o threads threads.cpp -pthread
//
//   o en versiones de GCC anteriores a 4.7
//
//      g++ -std=c++0x -o threads threads.cpp -pthread

#include <iostream>
#include <string>
#include <thread>

#include <sched.h>

void print_message_function(const std::string& message)
{
    for (int i = 0; i < 10; i++) {
        std::cout << message << "\n";
        for (int j = 0; j < 1000000; j++);
        
        // Invoca al planificador de CPU para dar una oportunidad a otros hilos
        sched_yield();
    }
}

int main()
{
    // Crear algunos hilos independientes cada uno de los cuales
    // ejecutará print_message_function()
    std::string message1 = "Hilo 1";
    std::thread thread1(print_message_function, message1);
    
    std::string message2 = "Hilo 2";
    std::thread thread2(print_message_function, message2);

    // Esperar a que los hilos terminen antes de que main() continúe.
    // Si no esperamos, corremos el riesgo de terminar el proceso y todos
    // sus hilos antes de que los hilos hayan terminado.
    thread1.join();
    thread2.join(); 

    return 0;
}
