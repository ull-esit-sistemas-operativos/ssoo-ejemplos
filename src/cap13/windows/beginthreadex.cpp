// beginthreadex.cpp - Ejemplo de creación de hilos con la API de Windows
//
//  Es la versión con la API de Windows del ejemplo de ../posix/pthreads.cpp
//
//  Compilar:
//
//      cl /std:c++latest /EHsc /utf-8 beginthreadex.cpp
//

#include <array>
#include <cerrno>
#include <cstdint>
#include <cstdlib>
#include <print>
#include <system_error>

#include <process.h>    // _beginthreadex() de la librería de ejecución de C (CRT) de Microsoft
#include <windows.h>    // Cabecera principal de la API de Windows

struct thread_args
{
    int id;
    int result;
};

// La función principal de un hilo de Windows devuelve un número, mientras que la de POSIX Threads devuelve un
// puntero. Por eso aquí no hace falta el campo 'result' de la versión de POSIX para tener dónde guardar el
// resultado: el valor cabe en el propio valor de retorno y se recoge con GetExitCodeThread().
//
// Es la firma que pide _beginthreadex(). La de CreateThread() es la misma con otros tipos:
// DWORD WINAPI thread_function(LPVOID arg).
unsigned __stdcall thread_function(void* arg)
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
    return static_cast<unsigned>(args->id);
}

int main()
{
    std::array<thread_args, 3> thread_args_list {{ { .id = 1, .result = 0 },
                                                   { .id = 2, .result = 0 },
                                                   { .id = 3, .result = 0 } }};
    std::array<HANDLE, 3> threads;

    // Crear 3 hilos dentro del proceso.
    //
    // La función de la API de Windows para crear hilos es CreateThread(). Pero Microsoft recomienda que los programas
    // en C o C++ usen _beginthreadex(), que llama a CreateThread() por debajo.
    //
    // _beginthreadex() recibe los mismos argumentos que CreateThread() y devuelve el mismo manejador del hilo, solo
    // que como un entero sin signo. Si falla, devuelve 0 y, como es una función de la librería de C de Microsoft, deja
    // el motivo en 'errno', en lugar de dejarlo en el sistema para que se consulte con GetLastError().
    for (size_t i = 0; i < threads.size(); ++i)
    {
        uintptr_t thread = _beginthreadex( nullptr, 0, thread_function, &thread_args_list[i], 0, nullptr );
        if (thread == 0)
        {
            std::println( stderr, "Error al crear el hilo: {}", std::generic_category().message(errno) );

            // Al terminar main() aquí, estaremos abortando la ejecución de los hilos que ya se hayan creado, si no
            // han terminado antes. Este caso es muy sencillo, así que no importa. Pero no suele ser buena idea no
            // dejar que los hilos tengan oportunidad de terminar por si mismos.
            return EXIT_FAILURE;
        }
        threads[i] = reinterpret_cast<HANDLE>(thread);
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
        // en POSIX Threads hace pthread_join() al recoger el hilo terminado. _beginthreadex() no lo cierra por su
        // cuenta, así que es responsabilidad nuestra.
        CloseHandle( threads[i] );
    }

    std::println( "Los hilos han terminado con {}, {} y {}", results[0], results[1], results[2] );

    return EXIT_SUCCESS;
}
