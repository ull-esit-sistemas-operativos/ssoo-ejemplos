// createprocess.cpp - Ejemplo del uso de CreateProcess() para ejecutar otros programas en C++
//
//  Compilar:
//
//      cl createprocess.cpp
//

#include <cstdlib>
#include <print>

#include <windows.h>

int main()
{
    TCHAR lpCommandLine[] = "C:\\Windows\\System32\\charmap.exe";
    STARTUPINFO si = { 
        .cb = sizeof(STARTUPINFO) 
    };
    PROCESS_INFORMATION pi = {};

    // Crear procesos hijo y comprobar si no se creó con éxito.
    if ( ! CreateProcess(
        nullptr,
        lpCommandLine,
        nullptr,
        nullptr,
        FALSE,
        0,
        nullptr,
        nullptr,
        &si,
        &pi ))
    {
        std::println( stderr, "Error ({}) al crear el proceso.", GetLastError() );
        return EXIT_FAILURE;
    }

    std::println( "[PADRE] El PID del nuevo proceso hijo es: {}", pi.dwProcessId );

    // Esperar hasta que el hijo termine.
    WaitForSingleObject( pi.hProcess, INFINITE );

    DWORD dwExitCode;
    GetExitCodeProcess( pi.hProcess, &dwExitCode );
    std::println( "[PADRE] El valor de salida del proceso hijo es: {}", dwExitCode );

    // Cerrar los manejadores del proceso y del hilo principal del proceso.
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );

    return EXIT_SUCCESS;
}
