// thread-mutex.cpp - Ejemplo del uso de mutex con POSIX pthread
//
//      g++ -o thread-mutex thread-mutex.cpp -lpthread

#include <iostream>
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
    std::cout << "Valor del contador: " << counter << std::endl;
//    pthread_mutex_unlock(&mutex);
}

int main()
{
    pthread_t thread1, thread2;

    // Crear algunos hilos independientes cada uno de los cuales
    // ejecutará increment_counter()
    int ret1 = pthread_create(&thread1, NULL, increment_counter, NULL);
    if (ret1)
        std::cout << "Error: pthread_create: " << ret1 << '\n';
    int ret2 = pthread_create(&thread2, NULL, increment_counter, NULL);
    if (ret2)
        std::cout << "Error: pthread_create: " << ret2 << '\n';

    // Esperar a que los hilos terminen antes de que main() continúe.
    // Si no esperamos, corremos el riesgo de terminar el proceso y todos
    // sus hilos antes de que los hilos hayan terminado.
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    
    std::cout << "Valor final del contador: " << counter << std::endl;

    return 0;
}
