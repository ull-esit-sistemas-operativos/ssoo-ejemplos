// createthread.cpp - Ejemplo de creación de hilos con la API de Windows
//
//  Es la versión con la API de Windows del ejemplo de ../posix/pthreads.cpp
//
//  Compilar:
//
//      cl /std:c++latest /EHsc /utf-8 createthread.cpp
//

#include <array>
#include <print>

#include <windows.h>    // Cabecera principal de la API de Windows

struct thread_args
{
    int id;
    int result;
};

// La función principal de un hilo de la API de Windows devuelve un DWORD, mientras que la de POSIX Threads devuelve un
// puntero. Por eso aquí no hace falta el campo 'result' de la versión de POSIX para tener dónde guardar el
// resultado: el valor cabe en el propio valor de retorno y se recoge con GetExitCodeThread().
DWORD WINAPI thread_function(LPVOID arg)
{
    thread_args* args = static_cast<thread_args*>(arg);

    std::println( "[Hilo {}] Creado. Identificador del sistema: {}", args->id, GetCurrentThreadId() );

    for(int i = 0; i < 5; ++i)
    {
        // Dormir el hilo para simular que hace trabajo
        Sleep(1000);
        std::println( "[Hilo {}] Ejecutada la iteración {}...", args->id, i );
    }
    std::println( "[Hilo {}] Terminado", args->id );

    args->result = args->id;
    return static_cast<DWORD>(args->id);
}

int main()
{
    std::array<thread_args, 3> thread_args_list {{ { .id = 1, .result = 0 },
                                                   { .id = 2, .result = 0 },
                                                   { .id = 3, .result = 0 } }};
    std::array<HANDLE, 3> threads;

    // Crear 3 hilos dentro del proceso.
    //
    // A diferencia de pthread_create(), que devuelve el código de error, CreateThread() devuelve el manejador del
    // hilo, o un manejador nulo si no pudo crearlo. El motivo, como en el resto de la API de Windows, hay que pedírselo
    // al sistema con GetLastError().
    for (size_t i = 0; i < threads.size(); ++i)
    {
        threads[i] = CreateThread( nullptr, 0, thread_function, &thread_args_list[i], 0, nullptr );
        if (threads[i] == nullptr)
        {
            std::println( stderr, "Error ({}) al crear el hilo.", GetLastError() );

            // Al terminar main() aquí, estaremos abortando la ejecución de los hilos que ya se hayan creado, si no
            // han terminado antes. Este caso es muy sencillo, así que no importa. Pero no suele ser buena idea no
            // dejar que los hilos tengan oportunidad de terminar por si mismos.
            return EXIT_FAILURE;
        }
    }

    // Esperar a que los hilos terminen antes de continuar.
    // Si salimos de main() sin esperar, el proceso terminará y todos los hilos morirán inmediatamente,
    // sin tener tiempo de terminar adecuadamente.
    //
    // Donde POSIX Threads necesita un pthread_join() por hilo, aquí se puede esperar por todos a la vez, porque un
    // hilo es un objeto del sistema como cualquier otro y WaitForMultipleObjects() espera por objetos.
    WaitForMultipleObjects( static_cast<DWORD>(threads.size()), threads.data(), TRUE, INFINITE );

    // Recoger el resultado de cada hilo. En POSIX Threads lo devuelve el propio pthread_join().
    std::array<DWORD, 3> results;
    for (size_t i = 0; i < threads.size(); ++i)
    {
        GetExitCodeThread( threads[i], &results[i] );

        // Los manejadores de los hilos hay que cerrarlos, como los de cualquier otro objeto del sistema. Es lo que
        // en POSIX Threads hace pthread_join() al recoger el hilo terminado.
        CloseHandle( threads[i] );
    }

    std::println( "Los hilos han terminado con {}, {} y {}", results[0], results[1], results[2] );

    return EXIT_SUCCESS;
}
