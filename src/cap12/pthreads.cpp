// threads.cpp - Ejemplo de creación de threads con POSIX Threads
//
//  Compilar:
//
//      g++ -I../ -I../../lib -o pthreads pthreads.cpp
//

#include <cerrno>
#include <cstring>
#include <print>

#include <pthread.h>
#include <unistd.h>

struct thread_args
{
    int id;
    int result;
};

void* thread_function(void* arg)
{
    thread_args* args = static_cast<thread_args*>(arg);

    std::println( "[Hilo {}] Creado. Manejador del sistema: 0x{:x}", args->id, pthread_self() );

    for(int i = 0; i < 5; ++i)
    {
        // Dormir el hilo para simular que hace trabajo
        sleep(1);
        std::println( "[Hilo {}] Ejecutada la iteración {}...", args->id, i );
    }
    std::println( "[Hilo {}] Terminado", args->id );

    args->result = args->id;
    return &args->result;
}

int main()
{
    int return_code = 0;
    pthread_t thread1, thread2, thread3;

    thread_args thread1_args { .id = 1 };
    thread_args thread2_args { .id = 2 };
    thread_args thread3_args { .id = 3 };
 
    // Crear 3 hilos dentro del proceso
    return_code = pthread_create( &thread1, nullptr, thread_function, &thread1_args );
    if (return_code)
    {
        std::println( stderr, "Error ({}) al crear el hilo: {}", return_code, std::strerror(return_code) );
        return EXIT_FAILURE;
    }
    
    return_code = pthread_create( &thread2, nullptr,  thread_function, &thread2_args );
    if (return_code)
    {
        std::println( stderr, "Error ({}) al crear el hilo: {}", return_code, std::strerror(return_code) );
        
        // Al terminar main() aquí, estaremos abortando la ejecución del primer hilo, si no ha terminado antes.
        // Este caso es muy sencillo, así que no importa. Pero no suele ser buena idea no dejar que los hilos tengan
        // oportunidad de terminar por si mismos.
        return EXIT_FAILURE;
    }

    return_code = pthread_create( &thread3, nullptr,  thread_function, &thread3_args );
    if (return_code)
    {
        std::println( stderr, "Error ({}) al crear el hilo: {}", return_code, std::strerror(return_code) );

        // Al terminar main() aquí, estaremos abortando la ejecución del primer y segundo hilo.
        return EXIT_FAILURE;
    }
    
    // Esperar a que los hilos terminen antes de continuar.
    // Si salimos de main() sin esperar, el proceso terminará y todos los hilos morirán inmediatamente,
    // sin tener tiempo de terminar adecuadamente. 
    int* thread1_result, *thread2_result, *thread3_result;

    pthread_join( thread1, reinterpret_cast<void**>(&thread1_result) );
    pthread_join( thread2, reinterpret_cast<void**>(&thread2_result) );
    pthread_join( thread3, reinterpret_cast<void**>(&thread3_result) );

    std::println( "Los hilos han terminado con {}, {} y {}", *thread1_result, *thread2_result, *thread3_result );

    return EXIT_SUCCESS;
}
