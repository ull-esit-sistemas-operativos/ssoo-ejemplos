// timeserver-windows.cpp - Funciones comunes a los ejemplos del servidor de tiempo.
//
//  Es la versión con la API de Windows de timeserver.cpp
//
//  POSIX no tiene un temporizador periódico sencillo: alarm() programa un único aviso, que llega en forma de la
//  señal SIGALRM y hay que volver a programar desde el propio manejador. La API de Windows sí lo tiene, así que aquí no
//  hay que reprogramar nada: se indica cada cuánto debe repetirse y el sistema se encarga.
//
//  La diferencia importante está en dónde se ejecuta el aviso. En POSIX es un manejador de señales, que interrumpe
//  al programa en cualquier punto, así que solo se pueden usar dentro las funciones seguras en señal es
//  --ni printf() ni std::println()--. Aquí el aviso llega en un hilo del sistema, no interrumpe a nadie, y se puede
//  usar con libertad la librería estándar de C++.
//

#include <chrono>
#include <print>
#include <system_error>

#include <windows.h>

#include "timeserver.hpp"

const DWORD ALARM_DEFAULT_TIME = 5000 /* ms */;

namespace
{
    HANDLE timer_handle = nullptr;
}

VOID CALLBACK alarm_timer_callback([[maybe_unused]] PVOID parameter, [[maybe_unused]] BOOLEAN timer_or_wait_fired)
{
    auto now = std::chrono::floor<std::chrono::seconds>( std::chrono::system_clock::now() );
    std::println( "{:%c}", now );
}

void start_alarm()
{
    // Al pasar el mismo valor como plazo inicial y como periodo, el temporizador salta por primera vez dentro de
    // ALARM_DEFAULT_TIME milisegundos y se repite cada ALARM_DEFAULT_TIME a partir de entonces.
    BOOL success = CreateTimerQueueTimer( &timer_handle, nullptr, alarm_timer_callback, nullptr,
                                          ALARM_DEFAULT_TIME, ALARM_DEFAULT_TIME, WT_EXECUTEDEFAULT );
    if (! success)
    {
        throw std::system_error( static_cast<int>(GetLastError()), std::system_category(),
                                 "Fallo en CreateTimerQueueTimer()" );
    }
}

void stop_alarm()
{
    if (timer_handle == nullptr) return;

    // Al pasar INVALID_HANDLE_VALUE como evento de terminación, la función espera a que acabe el aviso que pudiera
    // estar ejecutándose antes de devolver el control. Así no puede quedar ninguno a medias.
    DeleteTimerQueueTimer( nullptr, timer_handle, INVALID_HANDLE_VALUE );
    timer_handle = nullptr;
}
