// filelock-control.cpp - Programa de control del ejemplo del uso de bloqueos de archivos
//
//  El programa servidor utiliza un temporizador del sistema para mostrar periódicamente la hora. Además, crea un
//  archivo con el PID del proceso. Este archivo es bloqueado durante su creación para que solo un servidor pueda
//  escribir su PID en él. Otros servidores detectarán la situación y terminarán inmediatamente.
//
//  El programa de control puede usar este archivo para saber si el servidor está en ejecución y pedirle que termine.
//
//  Es la versión con la API Win32 del ejemplo de ../posix/filelock-control.cpp
//
//  Compilar:
//
//      cl /std:c++latest /EHsc /utf-8 filelock-control.cpp
//

#include <array>
#include <charconv>
#include <fstream>
#include <print>
#include <string>

#include <windows.h>    // Cabecera principal de la API Win32 del sistema operativo

#include "filelock-common.hpp"

int main()
{
    std::ifstream pidfile_stream{ PID_FILENAME };

    // Comprobamos si se pudo abrir el archivo con el PID.
    if ( ! pidfile_stream.is_open() )
    {
        // Con la API de la librería estándar de C++ no se pueden conocer más detalles sobre el motivo, como:
        // si el archivo no existe, no se tienen permisos suficientes, etc.
        std::println( stderr, "Error: No se puede abrir '{}'.\n"
                              "Quizás el servidor no se esté ejecutando o no se tengan permisos suficientes"
                              , PID_FILENAME );
        return EXIT_FAILURE;
    }

    std::array<char, 20> buffer;
    pidfile_stream.read( buffer.data(), sizeof(buffer) );

    // Convertir el PID leído como cadena en un número.
    DWORD server_pid = 0;
    auto [ptr, ec] = std::from_chars( buffer.data(), buffer.data() + pidfile_stream.gcount(), server_pid );

    if (ec != std::errc{} || server_pid == 0)
    {
        std::println( stderr, "Error: '{}' no contiene un PID válido.", PID_FILENAME );
        return EXIT_FAILURE;
    }

    // Comprobar que el proceso sigue vivo. Es para esto para lo que sirve el archivo con el PID: para saber si el
    // servidor está en ejecución.
    HANDLE process_handle = OpenProcess( PROCESS_QUERY_LIMITED_INFORMATION | SYNCHRONIZE, FALSE, server_pid );
    if (process_handle == nullptr || WaitForSingleObject( process_handle, 0 ) != WAIT_TIMEOUT)
    {
        std::println( stderr, "Error: El servidor con el PID {} ya no se está ejecutando.", server_pid );

        if (process_handle != nullptr) CloseHandle( process_handle );
        return EXIT_FAILURE;
    }
    CloseHandle( process_handle );

    std::println( "Cerrando el servidor..." );

    // Pedir al servidor que termine.
    //
    // Aquí es donde más se separan los dos sistemas. La versión de POSIX manda al proceso la señal SIGTERM con
    // kill(), y las señales son parte de la API del sistema. Windows no tiene nada equivalente: se puede matar un
    // proceso con TerminateProcess(), pero eso lo detiene en seco, sin darle ocasión de recoger sus cosas, que es
    // justo lo contrario de lo que hace SIGTERM.
    //
    // Lo habitual es usar un objeto del sistema por el que el servidor esté esperando. Aquí, un evento con nombre
    // que el servidor creó al arrancar.
    HANDLE quit_event = OpenEventA( EVENT_MODIFY_STATE, FALSE, QUIT_EVENT_NAME.c_str() );
    if (quit_event == nullptr)
    {
        std::println( stderr, "Error: No se pudo abrir el evento de terminación del proceso {}.", server_pid );
        return EXIT_FAILURE;
    }

    if (! SetEvent( quit_event ))
    {
        std::println( stderr, "Error: No se pudo avisar al proceso {}.", server_pid );

        CloseHandle( quit_event );
        return EXIT_FAILURE;
    }

    CloseHandle( quit_event );

    std::println( "¡Adiós!" );

    return EXIT_SUCCESS;
}
