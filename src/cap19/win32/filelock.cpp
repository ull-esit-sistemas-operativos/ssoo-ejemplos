// filelock.cpp - Ejemplo del uso de bloqueos de archivos
//
//  El programa servidor utiliza un temporizador del sistema para mostrar periódicamente la hora. Además, crea un
//  archivo con el PID del proceso. Este archivo es bloqueado durante su creación para que solo un servidor pueda
//  escribir su PID en él. Otros servidores detectarán la situación y terminarán inmediatamente.
//
//  El programa de control puede usar este archivo para saber si el servidor está en ejecución y pedirle que termine.
//
//  Es la versión con la API Win32 del ejemplo de ../posix/filelock.cpp
//
//  La versión de POSIX espera con sigwait() a que llegue SIGALRM --para mostrar la hora-- o SIGTERM o SIGINT --para
//  terminar--. En Windows no hay señales, así que el programa espera por dos objetos del sistema: un temporizador,
//  que se activa solo cada cierto tiempo, y un evento con nombre que activa el programa de control. La pulsación de
//  Ctrl+C, que es lo más parecido a SIGINT, llega a través de un manejador de la consola que activa ese mismo evento.
//
//  Compilar:
//
//      cl /std:c++latest /EHsc /utf-8 filelock.cpp
//

#include <array>
#include <chrono>
#include <print>
#include <string>
#include <system_error>

#include <windows.h>    // Cabecera principal de la API Win32 del sistema operativo

#include "filelock-common.hpp"
#include "pid_file.hpp"

const int ALARM_DEFAULT_TIME = 3 /* seg. */;

namespace
{
    // El manejador de la consola se ejecuta en un hilo aparte que crea el sistema, así que necesita llegar al evento
    // por el que espera el hilo principal.
    HANDLE quit_event = nullptr;
}

BOOL WINAPI console_ctrl_handler([[maybe_unused]] DWORD ctrl_type)
{
    // Ctrl+C, cerrar la ventana de la consola o cerrar la sesión hacen aquí el papel de SIGINT y SIGTERM.
    SetEvent( quit_event );

    // Devolver TRUE significa que la pulsación ya está atendida y el sistema no debe terminar el proceso por su
    // cuenta, que es lo que haría por omisión.
    return TRUE;
}

int protected_main()
{
    // Crear el archivo con el PID del proceso. Si ya hay otro servidor en ejecución, esto lanzará una excepción.
    examples::pid_file pidfile{ PID_FILENAME };

    // Crear el evento con el que el programa de control pedirá que el servidor termine.
    quit_event = CreateEventA( nullptr, FALSE, FALSE, QUIT_EVENT_NAME.c_str() );
    if (quit_event == nullptr)
    {
        throw std::system_error( static_cast<int>(GetLastError()), std::system_category(), "Fallo en CreateEvent()" );
    }

    SetConsoleCtrlHandler( console_ctrl_handler, TRUE );

    // Crear e iniciar el temporizador que hace el papel de la alarma.
    //
    // Al contrario que alarm(), que hay que volver a programar cada vez desde el manejador de la señal, un
    // temporizador esperable se puede programar para que se repita solo.
    HANDLE timer = CreateWaitableTimerA( nullptr, FALSE, nullptr );
    if (timer == nullptr)
    {
        throw std::system_error( static_cast<int>(GetLastError()), std::system_category(),
                                 "Fallo en CreateWaitableTimer()" );
    }

    // El plazo se indica en intervalos de 100 nanosegundos y, cuando es negativo, se cuenta desde ahora. El periodo,
    // en cambio, va en milisegundos.
    LARGE_INTEGER due_time;
    due_time.QuadPart = -static_cast<LONGLONG>(ALARM_DEFAULT_TIME) * 10000000LL;
    if (! SetWaitableTimer( timer, &due_time, ALARM_DEFAULT_TIME * 1000, nullptr, nullptr, FALSE ))
    {
        throw std::system_error( static_cast<int>(GetLastError()), std::system_category(),
                                 "Fallo en SetWaitableTimer()" );
    }

    // Esperar hasta que alguno de los dos objetos indique que hay algo que hacer.
    //
    // WaitForMultipleObjects() hace aquí el papel de sigwait(): deja el hilo principal dormido hasta que pasa algo y
    // devuelve qué ha sido, sin las limitaciones de un manejador asíncrono. Así se puede usar con total libertad el
    // resto de la librería estándar de C++ --como std::println() o las excepciones-- para tratar cada caso.
    std::array<HANDLE, 2> wait_objects { timer, quit_event };

    bool quit_app = false;
    while (!quit_app)
    {
        DWORD result = WaitForMultipleObjects( static_cast<DWORD>(wait_objects.size()), wait_objects.data(),
                                               FALSE, INFINITE );

        if (result == WAIT_OBJECT_0)
        {
            auto now = std::chrono::floor<std::chrono::seconds>( std::chrono::system_clock::now() );
            std::println( "{:%c}", now );

            // No hay que programar la siguiente alarma: el temporizador se repite solo.
        }
        else if (result == WAIT_OBJECT_0 + 1)
        {
            // Terminar el proceso.
            quit_app = true;
        }
        else
        {
            throw std::system_error( static_cast<int>(GetLastError()), std::system_category(),
                                     "Fallo en WaitForMultipleObjects()" );
        }
    }

    // Detener el temporizador.
    CancelWaitableTimer( timer );
    CloseHandle( timer );

    SetConsoleCtrlHandler( console_ctrl_handler, FALSE );
    CloseHandle( quit_event );

    // Vamos a salir del programa...
    std::println( "Ha llegado orden de terminar ¡Adiós!" );

    return EXIT_SUCCESS;
}

int main()
{
    try
    {
        return protected_main();
    }
    catch(std::system_error& e)
    {
        std::println( stderr, "Error ({}): {}", e.code().value(), e.what() );
    }
    catch(std::exception& e)
    {
        std::println( stderr, "Error: Excepción: {}", e.what() );
    }

    return EXIT_FAILURE;
}
