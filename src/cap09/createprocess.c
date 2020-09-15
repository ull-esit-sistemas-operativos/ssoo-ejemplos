// createprocess.c - Ejemplo del uso de CreateProcess() para ejecutar otros programas
//
//  Compilar:
//
//      cl createprocess.c
//

#include <windows.h>
#include <stdio.h>

int main()
{
    STARTUPINFO si = { sizeof(STARTUPINFO) };
    PROCESS_INFORMATION pi = {0};

    // Crear procesos hijo y comprobar si no se creó con éxito.
    if( ! CreateProcess(
        NULL,
        "C:\\Windows\\System32\\charmap.exe",
        NULL,
        NULL,
        FALSE,
        0,   
        NULL,
        NULL,
        &si,
        &pi ))
    {
        fprintf( stderr, "Error (%d) al crear el proceso.\n", GetLastError() );
        return 1;
    }

    printf( "[PADRE] El PID del nuevo proceso hijo es: %d\n", GetProcessId(pi.hProcess) );

    // Esperar hasta que el hijo termine.
    WaitForSingleObject( pi.hProcess, INFINITE );

    DWORD dwExitCode;
    GetExitCodeProcess( pi.hProcess, &dwExitCode );
    printf( "[PADRE] El valor de salida del proceso hijo es: %d\n", dwExitCode );

    // Cerrar los manejadores del proceso y del hilo principal del proceso. 
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );
}
