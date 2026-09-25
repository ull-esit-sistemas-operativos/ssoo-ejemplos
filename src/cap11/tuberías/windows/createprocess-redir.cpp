// createprocess-redir.cpp - Ejemplo del uso de tuberías para redirigir la E/S estándar
//
//  Es la versión con la API de Windows del ejemplo de ../posix/fork-redir.cpp
//
//  Compilar:
//
//      cl /std:c++latest /EHsc /utf-8 createprocess-redir.cpp
//

#include <algorithm>
#include <array>
#include <print>
#include <string>
#include <system_error>

#include <windows.h>    // Cabecera principal de la API de Windows

// Programa que se va a ejecutar en el proceso hijo y cuya salida se va a leer.
char COMMAND_LINE[] = "cmd.exe /c dir";

// Función para leer todo el contenido de un manejador de lectura.
std::string read_all( HANDLE handle )
{
    std::string stdout_buffer;
    std::array<char, 1024> read_buffer;

    // Para leer todo el contenido de la tubería, se lee en un bucle hasta que ReadFile() falla con
    // ERROR_BROKEN_PIPE, que es como la API de Windows avisa de que se ha cerrado el último manejador de escritura de
    // la tubería y, por tanto, de que no van a llegar más datos. En POSIX read() lo indica devolviendo 0.
    while (true)
    {
        DWORD bytes_read;
        if (! ReadFile( handle, read_buffer.data(), static_cast<DWORD>(read_buffer.size()), &bytes_read, nullptr ))
        {
            if (GetLastError() == ERROR_BROKEN_PIPE) break;

            throw std::system_error( static_cast<int>(GetLastError()), std::system_category(), "Fallo en ReadFile()" );
        }

        if (bytes_read == 0) break;

        stdout_buffer.append( read_buffer.data(), bytes_read );
    }

    return stdout_buffer;
}

int protected_main()
{
    // Crear una tubería para conectar el proceso hijo con el padre.
    //
    // Los manejadores de la tubería tienen que poder heredarse para que el hijo pueda usarlos. En POSIX no hace
    // falta pedirlo: el hijo de fork() hereda todos los descriptores del padre salvo que se diga lo contrario.
    SECURITY_ATTRIBUTES security_attributes = {
        .nLength = sizeof(SECURITY_ATTRIBUTES),
        .lpSecurityDescriptor = nullptr,
        .bInheritHandle = TRUE
    };

    HANDLE read_handle, write_handle;
    if (! CreatePipe( &read_handle, &write_handle, &security_attributes, 0 ))
    {
        throw std::system_error( static_cast<int>(GetLastError()), std::system_category(), "Fallo en CreatePipe()" );
    }

    // El hijo solo debe heredar el extremo de escritura. Si heredase también el de lectura, ese extremo seguiría
    // abierto y el padre nunca vería el final de los datos.
    if (! SetHandleInformation( read_handle, HANDLE_FLAG_INHERIT, 0 ))
    {
        throw std::system_error( static_cast<int>(GetLastError()), std::system_category(),
                                 "Fallo en SetHandleInformation()" );
    }

    // Un programa, como el que vamos a ejecutar, que quiere mostrar algo por pantalla, generalmente usa la salida
    // estándar. Si queremos que lo que escriba en ella vaya realmente a la entrada de la tubería, hay que decírselo
    // al sistema al crear el proceso.
    //
    // En POSIX esto se hace desde dentro del hijo, que después de fork() copia con dup2() el descriptor de la
    // tubería sobre su STDOUT_FILENO. En Windows no hay un momento equivalente --el hijo es un programa nuevo desde
    // el primer instante--, así que la redirección se pide por adelantado, en la estructura STARTUPINFO con la que
    // se crea el proceso.
    //
    // La marca STARTF_USESTDHANDLES es la que hace que el sistema mire los tres campos hStdXXX. Hay que rellenar los
    // tres, así que a la entrada y a la salida de error se les pasan las del propio proceso, para que el hijo las
    // comparta con el padre.
    STARTUPINFOA startup_info = {
        .cb = sizeof(STARTUPINFOA),
        .dwFlags = STARTF_USESTDHANDLES,
        .hStdInput = GetStdHandle( STD_INPUT_HANDLE ),
        .hStdOutput = write_handle,
        .hStdError = GetStdHandle( STD_ERROR_HANDLE )
    };
    PROCESS_INFORMATION process_info = {};

    // Crear el proceso hijo, que ejecuta directamente el programa que nos interesa. En POSIX hacen falta dos pasos,
    // fork() y exec(), y entre uno y otro está el código que prepara la redirección.
    if (! CreateProcessA( nullptr, COMMAND_LINE, nullptr, nullptr, TRUE, 0, nullptr, nullptr,
                          &startup_info, &process_info ))
    {
        throw std::system_error( static_cast<int>(GetLastError()), std::system_category(),
                                 "Fallo en CreateProcess()" );
    }

    // El manejador de escritura del hijo debe ser el único abierto, para que el padre sepa que no queda nada por
    // leer cuando el hijo muera.
    CloseHandle( write_handle );

    // Leer toda la salida del proceso hijo y cerrar el manejador de lectura.
    std::string stdout_buffer = read_all( read_handle );
    CloseHandle( read_handle );

    // Sabemos que el hijo ha terminado porque el otro extremo de la tubería se cerró. Aun así hay que esperar por él
    // para obtener su estado de salida y saber si terminó con éxito y, por tanto, si el contenido del búfer es
    // válido.
    WaitForSingleObject( process_info.hProcess, INFINITE );

    DWORD exit_code;
    if (! GetExitCodeProcess( process_info.hProcess, &exit_code ))
    {
        throw std::system_error( static_cast<int>(GetLastError()), std::system_category(),
                                 "Fallo en GetExitCodeProcess()" );
    }

    CloseHandle( process_info.hProcess );
    CloseHandle( process_info.hThread );

    if (exit_code != EXIT_SUCCESS)
    {
        std::println( "El comando 'dir' terminó inesperadamente." );
        return EXIT_FAILURE;
    }

    // Contar el número de líneas de la salida del comando.
    auto num_of_lines = std::count_if( stdout_buffer.begin(), stdout_buffer.end(),
        [](char c) { return c == '\n'; } );

    std::println( "La salida de 'dir' tiene {} líneas", num_of_lines );

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
