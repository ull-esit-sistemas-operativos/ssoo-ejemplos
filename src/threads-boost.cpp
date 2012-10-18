// thread-boost.cpp - Ejemplo de creación de threads con Boost.Thread
//
//      g++ -o thread-boost thread-boost.cpp -lboost_thread

#include <iostream>
#include <string>

#include <sched.h>

#include <boost/thread/thread.hpp>

void* print_message_function(std::string& message)
{
    for (int i = 0; i < 10; i++) {
        std::cout << message << '\n';
        for (int j = 0; j < 1000000; j++);
        sched_yield();      // Invoca al planificador
    }
}

int main()
{
    std::string message1("Hilo 1");
    std::string message2("Hilo 2");
    
    // Crear algunos hilos independientes cada uno de los cuales
    // ejecutará print_message_function()
    boost::thread thread1(print_message_function, message1);
    boost::thread thread2(print_message_function, message2);

    // Esperar a que los hilos terminen antes de que main() continúe.
    // Si no esperamos, corremos el riesgo de terminar el proceso y todos
    // sus hilos antes de que los hilos hayan terminado.
    thread1.join();
    thread2.join(); 

    return 0;
}
