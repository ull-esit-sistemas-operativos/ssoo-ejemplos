// threads-pthread.cpp - Ejemplo de creación de threads con la librería de
//                       hilos del sistema (POSIX pthread)
//
//      g++ -o threads-pthread threads-pthread.cpp -pthread

#include <iostream>
#include <cstring>

#include <pthread.h>
#include <sched.h>

void* print_message_function(void* ptr)
{
    char* message = static_cast<char*>(ptr);

    for (int i = 0; i < 10; i++) {
        std::cout << message << "\n";
        for (int j = 0; j < 1000000; j++);

        // Invoca al planificador de CPU para dar una oportunidad a otros hilos
        pthread_yield();
    }

    return NULL;
}

int main()
{
    // Crear algunos hilos independientes cada uno de los cuales
    // ejecutará print_message_function()
    pthread_t thread1;
    char message1[] = "Hilo 1";
    int error1 = pthread_create(&thread1, NULL, print_message_function, message1);
    if (error1)
        std::cerr << "Error: pthread_create: " << std::strerror(error1) << "\n";
    
    pthread_t thread2;
    char message2[] = "Hilo 2";
    int error2 = pthread_create(&thread2, NULL, print_message_function, message2);
    if (error2)
        std::cerr << "Error: pthread_create: " << std::strerror(error2) << "\n";

    // Esperar a que los hilos terminen antes de que main() continúe.
    // Si no esperamos, corremos el riesgo de terminar el proceso y todos
    // sus hilos antes de que los hilos hayan terminado.
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL); 

    return 0;
}
