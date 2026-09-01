// pid_file.hpp - Clase para el manejo del archivo de PID de un servidor
//

#pragma once

#include <array>
#include <cerrno>
#include <charconv>
#include <cstddef>
#include <format>
#include <stdexcept>
#include <string>
#include <system_error>

#include <unistd.h>     // Cabecera principal de la API POSIX del sistema operativo
#include <fcntl.h>      // Algunas operaciones del estándar POSIX con descriptores de archivo
                        // no están en <unistd.h> sino aquí. Por ejemplo open()
#include <sys/types.h>
#include <sys/stat.h>

namespace examples
{
    // Clase RAII que crea el archivo con el PID del proceso, bloqueándolo mientras lo escribe para evitar que dos
    // servidores se pisen entre sí, y se asegura de liberar el descriptor y de eliminar el archivo al terminar el
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
            // Abrir el archivo con el PID para escritura. Crearlo si no existe, con permisos 0600.
            fd_ = open( filename_.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR );
            if (fd_ < 0)
            {
                throw std::system_error( errno, std::system_category(), "Fallo en open()" );
            }

            try
            {
                lock_and_write_pid();
            }
            catch (...)
            {
                // No se pudo terminar de inicializar el objeto, así que su destructor no se va a ejecutar.
                // Hay que liberar aquí el descriptor abierto.
                close( fd_ );
                throw;
            }
        }

        // Asegurar que se liberan todos los recursos reservados en el constructor.
        ~pid_file()
        {
            close( fd_ );
            unlink( filename_.c_str() );
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
            if ( lockf( fd_, F_LOCK, 0 ) < 0 )
            {
                throw std::system_error( errno, std::system_category(), "Fallo en lockf()" );
            }
            // A partir de este punto, el proceso actual tiene acceso exclusivo al archivo.

            try
            {
                // Puede que ya haya otro servidor en ejecución. Intentar leer su PID.
                pid_t current_pid = read_pid();

                // Si se pudo leer el PID, comprobar si el proceso aun se está ejecutando.
                // Quizás murió sin tener tiempo de borrar el archivo.
                if (current_pid > 0 && process_exists( current_pid ))
                {
                    throw std::runtime_error( "Hay otro servidor en ejecución." );
                }

                // Si el archivo esta vacío o el proceso ya no existe, escribir el PID del proceso actual.
                // Mientras el proceso actual siga en ejecución, otros lo detectarán y terminarán prematuramente.
                write_pid( getpid() );
            }
            catch (...)
            {
                unlock();
                throw;
            }

            unlock();
        }

        // lockf() bloquea y desbloquea la región que va desde la posición actual del archivo hasta el final.
        // read_pid() y write_pid() mueven esa posición, así que hay que devolverla a 0 antes de desbloquear:
        // si no, se desbloquearía una región distinta de la que se bloqueó en lock_and_write_pid().
        void unlock()
        {
            lseek( fd_, 0, SEEK_SET );
            lockf( fd_, F_ULOCK, 0 );
        }

        pid_t read_pid()
        {
            std::array<char, 20> buffer;
            ssize_t bytes_read = read( fd_, buffer.data(), buffer.size() );
            if (bytes_read < 0)
            {
                throw std::system_error( errno, std::system_category(), "Fallo en read()" );
            }

            // Si el archivo está vacío o es muy grande, seguramente no contenga un PID válido.
            if (bytes_read == 0 || static_cast<std::size_t>(bytes_read) == buffer.size())
            {
                return -1;
            }

            int pid = -1;
            auto [ptr, ec] = std::from_chars( buffer.data(), buffer.data() + bytes_read, pid );

            // Si la conversión da valores de PID inválidos o no consume todos los caracteres leídos, seguramente no
            // contenga un PID válido.
            if (ec != std::errc{} || ptr != buffer.data() + bytes_read || pid <= 0)
            {
                return -1;
            }

            return pid;
        }

        void write_pid(pid_t pid)
        {
            std::string pid_string = std::to_string( pid );

            if ( ftruncate( fd_, 0 ) < 0 || write( fd_, pid_string.data(), pid_string.size() ) < 0 )
            {
                throw std::system_error( errno, std::system_category(), "Fallo al escribir el PID" );
            }
        }

        static bool process_exists(pid_t pid)
        {
            return access( std::format( "/proc/{}", pid ).c_str(), F_OK ) == 0;
        }

        std::string filename_;
        int fd_ = -1;
    };
} // namespace examples
