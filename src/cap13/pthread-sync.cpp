// threads-mutex-pthread.cpp - Ejemplo del uso de mutex con la librería de
//                             hilos del sistema (POSIX pthread)
//
//      g++ -o threads-mutex-pthread threads-mutex-pthread.cpp -pthread

#include <iostream>

#include <cstring>
#include <pthread.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int counter = 0;

void* increment_counter(void* ptr)
{
//    pthread_mutex_lock(&mutex);
    for (int i = 0; i < 1000000; i++) {
//        pthread_mutex_lock(&mutex);
        counter++;
//        pthread_mutex_unlock(&mutex);
    }
    std::cout << "Valor del contador: " << counter << "\n";
//    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main()
{
    // Crear algunos hilos independientes cada uno de los cuales
    // ejecutará increment_counter()
    pthread_t thread1;
    int error1 = pthread_create(&thread1, NULL, increment_counter, NULL);
    if (error1)
        std::cerr << "Error: pthread_create: " << std::strerror(error1) << "\n";
    
    pthread_t thread2;
    int error2 = pthread_create(&thread2, NULL, increment_counter, NULL);
    if (error2)
        std::cerr << "Error: pthread_create: " << std::strerror(error2) << "\n";

    // Esperar a que los hilos terminen antes de que main() continúe.
    // Si no esperamos, corremos el riesgo de terminar el proceso y todos
    // sus hilos antes de que los hilos hayan terminado.
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    
    std::cout << "Valor final del contador: " << counter << "\n";

    return 0;
}
