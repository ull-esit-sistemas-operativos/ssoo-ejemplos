// createprocess-pipe.cpp - Ejemplo del uso de tuberías para comunicar procesos
//
//  El programa solicita al usuario un número por la entrada estándar, lanza un proceso hijo para que calcule el
//  factorial y lee de la tubería que los conecta el resultado para utilizarlo.
//
//  Es la versión con la API Win32 del ejemplo de ../posix/fork-pipe.cpp
//
//  En Windows no hay nada parecido a fork(): no se puede duplicar el proceso actual, solo lanzar un programa. Para
//  que el hijo ejecute este mismo código, el padre se lanza a sí mismo con un argumento que le dice a la copia que
//  le toca hacer de hijo. Es lo más parecido a hacer un fork() seguido de un exec() del mismo programa.
//
//  Compilar:
//
//      cl /std:c++latest /EHsc /utf-8 /I..\..\.. createprocess-pipe.cpp
//

#include <array>
#include <charconv>
#include <format>
#include <print>
#include <string>
#include <string_view>
#include <system_error>

#include <windows.h>    // Cabecera principal de la API Win32 del sistema operativo

#include <common/factorial.hpp>

// Argumento con el que el proceso padre lanza la copia de sí mismo que hará de proceso hijo.
const std::string_view CHILD_ARGUMENT = "--hijo";

// Convertir un argumento de la línea de comandos en el número que representa.
template <typename Number>
Number parse_argument(const char* argument, const char* description)
{
    std::string_view text{ argument };

    Number value = 0;
    auto [ptr, ec] = std::from_chars( text.data(), text.data() + text.size(), value );
    if (ec != std::errc{} || ptr != text.data() + text.size())
    {
        throw std::runtime_error( std::format( "El argumento con {} no es válido: '{}'", description, argument ) );
    }

    return value;
}

int child_main(const char* handle_argument, const char* number_argument)
{
    // El hijo no es una copia del padre, así que no tiene ninguna de sus variables: todo lo que necesita saber tiene
    // que llegarle por la línea de comandos. En la versión de POSIX, en cambio, el hijo de fork() hereda una copia
    // de la memoria del padre y ya tiene dentro tanto el número como el descriptor de la tubería.
    HANDLE write_handle = reinterpret_cast<HANDLE>( parse_argument<uintptr_t>( handle_argument, "la tubería" ) );
    int number = parse_argument<int>( number_argument, "el número" );

    int factorial = calculate_factorial( number, "HIJO" );
    auto factorial_string = std::to_string( factorial );

    // Escribir en la tubería el resultado convertido a cadena sin el terminador nulo del final.
    DWORD bytes_written;
    if (! WriteFile( write_handle, factorial_string.c_str(), static_cast<DWORD>(factorial_string.length()),
                     &bytes_written, nullptr )
        || bytes_written < factorial_string.length())
    {
        std::println( stderr, "[HIJO] Error ({}) al escribir en la tubería.", GetLastError() );

        CloseHandle( write_handle );
        return EXIT_FAILURE;
    }

    // Al terminar el proceso todos los recursos se liberan y la entrada de tubería del hijo se cierra. Si ese es el
    // último manejador abierto de la entrada a la tubería, el padre verá que no queda nada más que leer.
    //
    // Aquí el hijo sí puede terminar con 'return', al contrario que en la versión de POSIX, donde hay que usar
    // _exit(). Este proceso no es una copia del padre, sino un programa recién arrancado con su propio estado, así
    // que no hay búferes heredados que se vayan a vaciar por duplicado.
    CloseHandle( write_handle );
    return EXIT_SUCCESS;
}

int parent_main()
{
    int number = get_user_input( "PADRE" );

    // Crear una tubería.
    //
    // Hay que pedir explícitamente que los manejadores de la tubería se puedan heredar. En POSIX no hace falta: el
    // hijo de fork() es una copia del padre y hereda todos sus descriptores de archivo salvo que se pida lo
    // contrario.
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

    // De los dos extremos de la tubería el hijo solo necesita el de escritura, para devolver el resultado. Si
    // heredase también el de lectura, ese extremo seguiría abierto y el padre nunca vería el final de los datos.
    //
    // Es el equivalente del close(fds[0]) que hace el hijo en la versión de POSIX, solo que al revés: allí el hijo
    // cierra lo que no necesita y aquí el padre decide de antemano qué se hereda y qué no.
    if (! SetHandleInformation( read_handle, HANDLE_FLAG_INHERIT, 0 ))
    {
        throw std::system_error( static_cast<int>(GetLastError()), std::system_category(),
                                 "Fallo en SetHandleInformation()" );
    }

    // Averiguar la ruta de este mismo programa, que es el que se va a lanzar como proceso hijo.
    std::array<char, MAX_PATH> program_path;
    if (GetModuleFileNameA( nullptr, program_path.data(), static_cast<DWORD>(program_path.size()) ) == 0)
    {
        throw std::system_error( static_cast<int>(GetLastError()), std::system_category(),
                                 "Fallo en GetModuleFileName()" );
    }

    // El hijo hereda el manejador, pero no sabe cuál de todos es, así que hay que decírselo. El número que
    // identifica al manejador vale en el proceso hijo igual que en el padre.
    std::string command_line = std::format( "\"{}\" {} {} {}", program_path.data(), CHILD_ARGUMENT,
                                            reinterpret_cast<uintptr_t>(write_handle), number );

    // Crear el proceso hijo para el cálculo del factorial.
    STARTUPINFOA startup_info = { .cb = sizeof(STARTUPINFOA) };
    PROCESS_INFORMATION process_info = {};

    // El quinto parámetro, bInheritHandles, tiene que ser TRUE para que el hijo herede los manejadores marcados como
    // heredables. Si fuese FALSE no heredaría ninguno, por mucho que se hayan marcado.
    if (! CreateProcessA( nullptr, command_line.data(), nullptr, nullptr, TRUE, 0, nullptr, nullptr,
                          &startup_info, &process_info ))
    {
        throw std::system_error( static_cast<int>(GetLastError()), std::system_category(),
                                 "Fallo en CreateProcess()" );
    }

    // El padre solo necesita el manejador de lectura para obtener el resultado del hijo. Además, el manejador de
    // escritura del hijo debe ser el único abierto, para saber que no queda nada por leer cuando el hijo muera.
    //
    // Cerramos el manejador de escritura.
    CloseHandle( write_handle );

    // ReadFile() lee los bytes disponibles en la tubería. Para leer todo hay que leer hasta que falle con
    // ERROR_BROKEN_PIPE, que es como la API Win32 avisa de que se ha cerrado el último extremo de escritura. En
    // POSIX read() lo indica devolviendo 0, un fin de archivo como el de cualquier otro descriptor.
    std::array<char, 255> read_buffer;
    char* read_buffer_begin = read_buffer.data();
    char* read_buffer_end = read_buffer.data() + read_buffer.size();
    DWORD bytes_read = 1;

    while (bytes_read > 0)
    {
        if (! ReadFile( read_handle, read_buffer_begin, static_cast<DWORD>(read_buffer_end - read_buffer_begin),
                        &bytes_read, nullptr ))
        {
            if (GetLastError() == ERROR_BROKEN_PIPE) break;

            throw std::system_error( static_cast<int>(GetLastError()), std::system_category(), "Fallo en ReadFile()" );
        }

        read_buffer_begin += bytes_read;
    }

    // Hemos leído hasta el final. Ya no necesitamos el manejador de lectura.
    CloseHandle( read_handle );

    // Sabemos que el hijo ha terminado porque el otro extremo de la tubería se cerró. Aun así hay que esperar por él
    // para obtener su estado de salida y saber si terminó con éxito y, por tanto, si el contenido del búfer es
    // válido. A diferencia de POSIX, en Windows no hay procesos zombi de los que preocuparse.
    WaitForSingleObject( process_info.hProcess, INFINITE );

    DWORD exit_code;
    if (! GetExitCodeProcess( process_info.hProcess, &exit_code ))
    {
        throw std::system_error( static_cast<int>(GetLastError()), std::system_category(),
                                 "Fallo en GetExitCodeProcess()" );
    }

    // Cerrar los manejadores del proceso y del hilo principal del proceso.
    CloseHandle( process_info.hProcess );
    CloseHandle( process_info.hThread );

    if (exit_code != EXIT_SUCCESS)
    {
        std::println( stderr, "Error: La tarea terminó inesperadamente." );
        return EXIT_FAILURE;
    }

    std::string factorial( read_buffer.data(), read_buffer_begin );
    std::println( "[PADRE] El factorial de {} es {}", number, factorial );

    return EXIT_SUCCESS;
}

int protected_main(int argc, char* argv[])
{
    // Si llega el argumento convenido, esta es la copia del programa que el padre ha lanzado como proceso hijo.
    if (argc == 4 && CHILD_ARGUMENT == argv[1])
    {
        return child_main( argv[2], argv[3] );
    }

    return parent_main();
}

int main(int argc, char* argv[])
{
    try
    {
        return protected_main(argc, argv);
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
