// namedpipe.cpp - Ejemplo del uso de tuberías con nombre para comunicar procesos
//
//  El programa de ejemplo utiliza un temporizador del sistema para mostrar periódicamente la hora. Además, crea una
//  tubería con nombre a la que puede conectarse el programa de control para darle órdenes.
//
//  Es la versión con la API Win32 del ejemplo de ../posix/fifo.cpp
//
//  Compilar:
//
//      cl /std:c++latest /EHsc /utf-8 /I..\..\.. namedpipe.cpp ..\..\..\common\timeserver-win32.cpp
//

#include <array>
#include <print>
#include <string>
#include <string_view>
#include <system_error>

#include <windows.h>    // Cabecera principal de la API Win32 del sistema operativo

#include <common/timeserver.hpp>
#include "namedpipe-common.hpp"

const DWORD MAX_COMMAND_SIZE = 100;

int protected_main()
{
    // Crear la tubería con nombre.
    //
    // En POSIX la tubería se crea con mkfifo() y después se abre con open(), como cualquier otro archivo. Aquí
    // CreateNamedPipe() hace las dos cosas a la vez y, además, hay que decirle de antemano cómo se va a usar: solo
    // para recibir datos (PIPE_ACCESS_INBOUND), tratándolos como una secuencia de bytes (PIPE_TYPE_BYTE) y
    // bloqueando al proceso mientras espera (PIPE_WAIT).
    HANDLE pipe_handle = CreateNamedPipeA( CONTROL_PIPE_NAME.c_str(),
                                           PIPE_ACCESS_INBOUND,
                                           PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
                                           PIPE_UNLIMITED_INSTANCES,
                                           MAX_COMMAND_SIZE, MAX_COMMAND_SIZE,
                                           0, nullptr );
    if (pipe_handle == INVALID_HANDLE_VALUE)
    {
        throw std::system_error( static_cast<int>(GetLastError()), std::system_category(),
                                 "Fallo en CreateNamedPipe()" );
    }

    std::println( "Escuchando en la tubería de control '{}'...", CONTROL_PIPE_NAME );

    // Comenzar a mostrar la hora periódicamente.
    start_alarm();

    // Leer de la tubería de control los comandos e interpretarlos.
    bool quit_app = false;
    while (!quit_app)
    {
        // Esperar a que un cliente se conecte.
        //
        // Este paso no existe en POSIX, donde la tubería no distingue clientes: cualquiera que abra el archivo
        // escribe en ella y lo que llega se mezcla. Las tuberías con nombre de la API Win32 están orientadas a la
        // conexión, así que el servidor atiende a un cliente cada vez y sabe cuándo se va.
        if (! ConnectNamedPipe( pipe_handle, nullptr ) && GetLastError() != ERROR_PIPE_CONNECTED)
        {
            throw std::system_error( static_cast<int>(GetLastError()), std::system_category(),
                                     "Fallo en ConnectNamedPipe()" );
        }

        // Leer los comandos que envíe este cliente, hasta que cierre su extremo de la tubería.
        while (!quit_app)
        {
            std::array<char, MAX_COMMAND_SIZE> buffer;

            DWORD bytes_read;
            if (! ReadFile( pipe_handle, buffer.data(), static_cast<DWORD>(buffer.size()), &bytes_read, nullptr ))
            {
                // ERROR_BROKEN_PIPE es la forma que tiene la API Win32 de avisar de que el cliente ha cerrado su
                // extremo. En POSIX eso es el fin de archivo que devuelve read(), o el nullptr de fgets().
                if (GetLastError() == ERROR_BROKEN_PIPE) break;

                throw std::system_error( static_cast<int>(GetLastError()), std::system_category(),
                                         "Fallo en ReadFile()" );
            }

            if (bytes_read == 0) break;

            std::string_view command{ buffer.data(), bytes_read };

            // Eliminar el salto de línea del final, que en Windows son dos caracteres.
            while (! command.empty() && (command.back() == '\n' || command.back() == '\r'))
            {
                command.remove_suffix( 1 );
            }

            if ( QUIT_COMMAND == command )
            {
                std::println( "Ha llegado orden de terminar ¡Adiós!" );
                quit_app = true;
            }
            // Aquí va código para detectar e interpretar más comandos...
            //
            else
            {
                std::println( "Comando de control no reconocido: '{}'", command );
            }
        }

        // Despedir al cliente para poder atender al siguiente.
        DisconnectNamedPipe( pipe_handle );
    }

    // Parar de mostrar la hora periódicamente.
    stop_alarm();

    // Cerrar la tubería de control. Nadie más podrá conectarse.
    //
    // No hace falta nada parecido al unlink() de la versión de POSIX: la tubería no es un archivo del sistema de
    // archivos que haya que borrar, sino un objeto del sistema que desaparece al cerrarse su último manejador.
    CloseHandle( pipe_handle );

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
