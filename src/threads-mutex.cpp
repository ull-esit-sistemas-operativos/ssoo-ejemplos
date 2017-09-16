// threads-mutex.cpp - Ejemplo del uso de mutex en C++
//
//      g++ -std=c++11 -o threads-mutex threads-mutex.cpp -pthread
//
//   o en versiones de GCC anteriores a 4.7
//
//      g++ -std=c++0x -o threads-mutex threads-mutex.cpp -pthread

#include <iostream>
#include <mutex>
#include <thread>

std::mutex mutex;
int counter = 0;

void increment_counter()
{
//    mutex.lock();
    for (int i = 0; i < 1000000; i++) {
//        mutex.lock();
        counter++;
//        mutex.unlock();
    }
    std::cout << "Valor del contador: " << counter << std::endl;
//    mutex.unlock();
}

int main()
{
    // Crear algunos hilos independientes cada uno de los cuales
    // ejecutará increment_counter()
    std::thread thread1(increment_counter);
    std::thread thread2(increment_counter);

    // Esperar a que los hilos terminen antes de que main() continúe.
    // Si no esperamos, corremos el riesgo de terminar el proceso y todos
    // sus hilos antes de que los hilos hayan terminado.
    thread1.join();
    thread2.join();
 
    std::cout << "Valor final del contador: " << counter << "\n";

    return 0;
}
