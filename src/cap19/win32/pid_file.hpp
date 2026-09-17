// pid_file.hpp - Clase para el manejo del archivo de PID de un servidor
//
//  Es la versión con la API Win32 de ../posix/pid_file.hpp
//

#pragma once

#include <array>
#include <charconv>
#include <cstddef>
#include <stdexcept>
#include <string>
#include <system_error>

#include <windows.h>    // Cabecera principal de la API Win32 del sistema operativo

namespace examples
{
    // Clase RAII que crea el archivo con el PID del proceso, bloqueándolo mientras lo escribe para evitar que dos
    // servidores se pisen entre sí, y se asegura de liberar el manejador y de eliminar el archivo al terminar el
    // programa, incluso si termina por una excepción.
    //
    // Si ya hay otro servidor en ejecución --hay un PID en el archivo y el proceso todavía existe-- el constructor
    // lanza una excepción.
    class pid_file
    {
    public:
        explicit pid_file(const std::string& filename)
            : filename_{filename}
        {
            // Abrir el archivo con el PID para lectura y escritura. Crearlo si no existe.
            //
            // Hay que permitir que otros procesos lo abran a la vez --FILE_SHARE_READ y FILE_SHARE_WRITE-- porque de
            // eso trata el ejemplo: de que varios servidores lleguen al mismo tiempo y sea el bloqueo, y no el hecho
            // de tener el archivo abierto, lo que decida cuál se queda. Windows, al contrario que POSIX, da acceso
            // exclusivo al abrir si no se dice lo contrario.
            handle_ = CreateFileA( filename_.c_str(), GENERIC_READ | GENERIC_WRITE,
                                   FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_ALWAYS,
                                   FILE_ATTRIBUTE_NORMAL, nullptr );
            if (handle_ == INVALID_HANDLE_VALUE)
            {
                throw std::system_error( static_cast<int>(GetLastError()), std::system_category(),
                                         "Fallo en CreateFile()" );
            }

            try
            {
                lock_and_write_pid();
            }
            catch (...)
            {
                // No se pudo terminar de inicializar el objeto, así que su destructor no se va a ejecutar.
                // Hay que liberar aquí el manejador abierto.
                CloseHandle( handle_ );
                throw;
            }
        }

        // Asegurar que se liberan todos los recursos reservados en el constructor.
        ~pid_file()
        {
            // En Windows hay que cerrar el archivo antes de borrarlo. Un archivo abierto solo se puede borrar si
            // quien lo abrió lo permitió con FILE_SHARE_DELETE.
            CloseHandle( handle_ );
            DeleteFileA( filename_.c_str() );
        }

        pid_file(const pid_file&) = delete;
        pid_file& operator=(const pid_file&) = delete;

    private:
        // Varios servidores pueden haberse lanzado al mismo tiempo. Solo uno debe poder escribir su PID en el
        // archivo y continuar ejecutándose. El resto deben detectar la situación y terminar inmediatamente.
        //
        // Para evitar que se pisen unos a otros escribiendo su PID en el archivo, adquirimos el bloqueo mientras
        // comprobamos si hay otro servidor en ejecución y, si no lo hay, escribimos nuestro PID. El bloqueo se
        // libera antes de devolver el control, tanto si hay éxito como si se lanza una excepción: solo hace falta
        // mientras se hace la comprobación, no durante el resto de la vida del programa.
        void lock_and_write_pid()
        {
            // LockFileEx() bloquea un rango de bytes del archivo, no desde la posición actual hasta el final como
            // lockf() en POSIX. Se indica el rango en la estructura OVERLAPPED --aquí, desde el byte 0-- y el tamaño
            // en dos mitades de 32 bits. Al pedir MAXDWORD en las dos se bloquea el archivo entero.
            //
            // Además, los bloqueos de Windows son obligatorios: el sistema impide de verdad que otro proceso escriba
            // en el rango bloqueado. Los de POSIX que usa lockf() son solo consultivos, y únicamente funcionan si
            // todos los que tocan el archivo se molestan en comprobarlos.
            OVERLAPPED overlapped = {};
            if (! LockFileEx( handle_, LOCKFILE_EXCLUSIVE_LOCK, 0, MAXDWORD, MAXDWORD, &overlapped ))
            {
                throw std::system_error( static_cast<int>(GetLastError()), std::system_category(),
                                         "Fallo en LockFileEx()" );
            }
            // A partir de este punto, el proceso actual tiene acceso exclusivo al archivo.

            try
            {
                // Puede que ya haya otro servidor en ejecución. Intentar leer su PID.
                DWORD current_pid = read_pid();

                // Si se pudo leer el PID, comprobar si el proceso aun se está ejecutando.
                // Quizás murió sin tener tiempo de borrar el archivo.
                if (current_pid > 0 && process_exists( current_pid ))
                {
                    throw std::runtime_error( "Hay otro servidor en ejecución." );
                }

                // Si el archivo esta vacío o el proceso ya no existe, escribir el PID del proceso actual.
                // Mientras el proceso actual siga en ejecución, otros lo detectarán y terminarán prematuramente.
                write_pid( GetCurrentProcessId() );
            }
            catch (...)
            {
                unlock();
                throw;
            }

            unlock();
        }

        // Al contrario que lockf(), LockFileEx() no bloquea a partir de la posición del archivo, sino un rango
        // indicado aparte. Así que aquí no hay que devolver la posición a 0 antes de desbloquear, como sí hay que
        // hacer en la versión de POSIX: basta con dar el mismo rango que se bloqueó.
        void unlock()
        {
            OVERLAPPED overlapped = {};
            UnlockFileEx( handle_, 0, MAXDWORD, MAXDWORD, &overlapped );
        }

        DWORD read_pid()
        {
            SetFilePointer( handle_, 0, nullptr, FILE_BEGIN );

            std::array<char, 20> buffer;
            DWORD bytes_read;
            if (! ReadFile( handle_, buffer.data(), static_cast<DWORD>(buffer.size()), &bytes_read, nullptr ))
            {
                throw std::system_error( static_cast<int>(GetLastError()), std::system_category(),
                                         "Fallo en ReadFile()" );
            }

            // Si el archivo está vacío o es muy grande, seguramente no contenga un PID válido.
            if (bytes_read == 0 || static_cast<std::size_t>(bytes_read) == buffer.size())
            {
                return 0;
            }

            DWORD pid = 0;
            auto [ptr, ec] = std::from_chars( buffer.data(), buffer.data() + bytes_read, pid );

            // Si la conversión da valores de PID inválidos o no consume todos los caracteres leídos, seguramente no
            // contenga un PID válido.
            if (ec != std::errc{} || ptr != buffer.data() + bytes_read || pid == 0)
            {
                return 0;
            }

            return pid;
        }

        void write_pid(DWORD pid)
        {
            std::string pid_string = std::to_string( pid );

            // read_pid() ha dejado la posición del archivo tras el PID que leyera, así que hay que volver al
            // principio antes de escribir. SetEndOfFile() recorta el archivo por la posición actual, que es lo que
            // en POSIX hace ftruncate() con el tamaño que se le indique.
            DWORD bytes_written;
            if (SetFilePointer( handle_, 0, nullptr, FILE_BEGIN ) == INVALID_SET_FILE_POINTER
                || ! SetEndOfFile( handle_ )
                || ! WriteFile( handle_, pid_string.data(), static_cast<DWORD>(pid_string.size()),
                                &bytes_written, nullptr ))
            {
                throw std::system_error( static_cast<int>(GetLastError()), std::system_category(),
                                         "Fallo al escribir el PID" );
            }
        }

        // En POSIX basta con mirar si existe el directorio del proceso en /proc. Aquí hay que pedirle al sistema un
        // manejador del proceso y preguntar si sigue vivo: un proceso que ha terminado conserva su identificador
        // hasta que se cierra el último manejador que lo apuntaba.
        static bool process_exists(DWORD pid)
        {
            HANDLE process_handle = OpenProcess( PROCESS_QUERY_LIMITED_INFORMATION | SYNCHRONIZE, FALSE, pid );
            if (process_handle == nullptr)
            {
                return false;
            }

            // Si la espera vence inmediatamente es que el proceso no está señalizado, o sea, que sigue vivo.
            bool running = WaitForSingleObject( process_handle, 0 ) == WAIT_TIMEOUT;

            CloseHandle( process_handle );
            return running;
        }

        std::string filename_;
        HANDLE handle_ = INVALID_HANDLE_VALUE;
    };
} // namespace examples
