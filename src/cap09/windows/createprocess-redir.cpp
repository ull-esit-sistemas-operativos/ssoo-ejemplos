// createprocess-redir.cpp - Ejemplo del uso de CreateProcess() para redirigir la salida de otro programa a un archivo
//
//  Es la versión con la API de Windows del ejemplo de ../posix/fork-redir.cpp
//
//  Compilar:
//
//      cl /std:c++latest /utf-8 createprocess-redir.cpp
//

#include <cstdlib>
#include <print>

#include <windows.h>

int main()
{
    // Abrir el archivo en modo escritura, porque vamos a redirigir a él la salida estándar del hijo.
    // Si no existe, se crea y si existe, se trunca.
    //
    // El manejador tiene que poder heredarse para que el hijo pueda usarlo. En POSIX no hace falta pedirlo: el hijo
    // de fork() hereda todos los descriptores del padre salvo que se diga lo contrario.
    SECURITY_ATTRIBUTES sa = {
        .nLength = sizeof(SECURITY_ATTRIBUTES),
        .lpSecurityDescriptor = nullptr,
        .bInheritHandle = TRUE
    };

    HANDLE hFile = CreateFileA(
        "salida.txt",
        GENERIC_WRITE,
        FILE_SHARE_READ,
        &sa,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        nullptr );

    if ( hFile == INVALID_HANDLE_VALUE )
    {
        std::println( stderr, "Error ({}) al abrir el archivo.", GetLastError() );
        return EXIT_FAILURE;
    }

    // En POSIX la redirección se hace desde dentro del hijo, que entre fork() y exec() copia con dup2() el
    // descriptor del archivo sobre su STDOUT_FILENO. En Windows no hay un momento equivalente --el hijo es un
    // programa nuevo desde el primer instante--, así que la redirección se pide por adelantado, en la estructura
    // STARTUPINFO con la que se crea el proceso.
    //
    // La marca STARTF_USESTDHANDLES es la que hace que el sistema mire los tres campos hStdXXX. Hay que rellenar los
    // tres, así que a la entrada y a la salida de error se les pasan las del propio proceso.
    STARTUPINFOA si = {
        .cb = sizeof(STARTUPINFOA),
        .dwFlags = STARTF_USESTDHANDLES,
        .hStdInput = GetStdHandle( STD_INPUT_HANDLE ),
        .hStdOutput = hFile,
        .hStdError = GetStdHandle( STD_ERROR_HANDLE )
    };
    PROCESS_INFORMATION pi = {};

    char lpCommandLine[] = "cmd.exe /c dir";

    std::println( "[PADRE] Voy a ejecutar el comando 'dir' con la salida redirigida a 'salida.txt'" );

    // Crear proceso hijo y comprobar si no se creó con éxito.
    // El quinto argumento, bInheritHandles, tiene que ser TRUE para que el hijo herede el manejador del archivo.
    BOOL bSuccess = CreateProcessA(
        nullptr,
        lpCommandLine,
        nullptr,
        nullptr,
        TRUE,
        0,
        nullptr,
        nullptr,
        &si,
        &pi );

    // El hijo ya tiene su propia copia del manejador del archivo, así que el padre puede cerrar la suya.
    CloseHandle( hFile );

    if ( ! bSuccess )
    {
        std::println( stderr, "Error ({}) al crear el proceso.", GetLastError() );
        return EXIT_FAILURE;
    }

    // Esperar hasta que el hijo termine. La salida estándar del padre no ha cambiado.
    std::println( "[PADRE] Voy a esperar a que mi hijo termine..." );
    WaitForSingleObject( pi.hProcess, INFINITE );

    DWORD dwExitCode;
    GetExitCodeProcess( pi.hProcess, &dwExitCode );

    // Cerrar los manejadores del proceso y del hilo principal del proceso.
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );

    if ( dwExitCode != EXIT_SUCCESS )
    {
        std::println( "[PADRE] El comando 'dir' terminó inesperadamente." );
        return EXIT_FAILURE;
    }

    std::println( "[PADRE] La salida de 'dir' está en el archivo 'salida.txt'" );
    return EXIT_SUCCESS;
}
