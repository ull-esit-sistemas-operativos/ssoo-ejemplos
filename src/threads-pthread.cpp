// threads-pthread.cpp - Ejemplo de creación de threads con la librería de
//                       hilos del sistema (POSIX pthread)
//
//      g++ -o threads-pthread threads-pthread.cpp -lpthread

#include <iostream>

#include <pthread.h>
#include <sched.h>

void* print_message_function(void* ptr)
{
    char* message = (char*) ptr;

    for (int i = 0; i < 10; i++) {
        std::cout << message << '\n';
        for (int j = 0; j < 1000000; j++);
        sched_yield();  // invoca al planificador
    }

    return NULL;
}

int main()
{
    pthread_t thread1, thread2;
    const char* message1 = "Hilo 1";
    const char* message2 = "Hilo 2";

    // Crear algunos hilos independientes cada uno de los cuales
    // ejecutará print_message_function()
    int ret1 = pthread_create(&thread1, NULL, print_message_function,
                              (void*)message1);
    if (ret1)
        std::cout << "Error: pthread_create: " << ret1 << '\n';
    int ret2 = pthread_create(&thread2, NULL, print_message_function,
                              (void*)message2);
    if (ret2)
        std::cout << "Error: pthread_create: " << ret2 << '\n';

    // Esperar a que los hilos terminen antes de que main() continúe.
    // Si no esperamos, corremos el riesgo de terminar el proceso y todos
    // sus hilos antes de que los hilos hayan terminado.
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL); 

    return 0;
}
