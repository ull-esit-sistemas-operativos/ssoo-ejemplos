// memory_map.hpp -  Clase de archivos mapeados en memoria
//

#pragma once

#include <unistd.h>     // Cabecera principal de la API POSIX del sistema operativo
#include <fcntl.h>      // Cabecera para open()
#include <sys/mman.h>   // Cabecera para mmap()
#include <sys/types.h>
#include <sys/stat.h>

#include <cerrno>
#include <cstring>
#include <functional>
#include <memory>
#include <stdexcept>
#include <system_error>

namespace examples
{
    class memory_map
    {
    public:

        enum class open_mode
        {
            read_write = O_RDWR,
            read_only = O_RDONLY,
            write_only = O_WRONLY 
        };

        memory_map() {}

        memory_map(const std::string& pathname, open_mode mode)
        {
            int flags = static_cast<int>(mode);

            // Abrir el archivo indicado en 'pathname'.
            fd_ = open( pathname.c_str(), flags, 0666 );
            if (fd_ < 0)
            {
                throw std::system_error( errno, std::system_category(), "Fallo en open()" );
            }
        }

        // Asegurar que se liberan todos los recursos reservados en el constructor.
        ~memory_map()
        {
            if (fd_ >= 0) 
            {
                close( fd_ );
            }
        }

        // map() - Mapear en la memoria del proceso 'num' objetos de tipo T (es decir, 'sizeof(T) * num' bytes) del
        //  archivo abierto en 'fd_'. Así se puede mapear en la memoria un solo elemento T o un array de elementos T.
        //
        // NOTAS:
        //  Se usa mmap() para reservar una región de la memoria virtual del proceso y mapear en ella el archivo.
        //  Mientras que para liberar la región reservada se usa munmap().

        //  En C++ no es adecuado depender de "acordamos de llamar a munmap()" cuando ya no necesitamos la memoria
        //  mapeada. En su lugar, debemos usar punteros inteligentes (como std::unique_ptr) para que se llame
        //  automáticamente a munmap() cuando el puntero va a ser destruido.

        template <typename T>
        using mmap_deleter_func = std::function<void(T*)>;

        template<typename T>
        std::unique_ptr<T, std::function<void(T*)>> map( int prot, size_t num = 1, off_t offset = 0)
        {
            if (fd_ < 0) throw std::runtime_error( "El objeto no contiene un descriptor válido." );

            void* mapped_mem = mmap( nullptr, num * sizeof(T), prot, MAP_SHARED, fd_, offset );
            if (mapped_mem == MAP_FAILED)
            {
                throw std::system_error( errno, std::system_category(), "Fallo en mmap()" );
            }

            // Función para liberar la memoria mapeada.
            mmap_deleter_func<T> mmap_deleter = [num](T* addr)
            {
                munmap(
                    addr,               // Puntero a la región a liberar (devuelto por mmap())
                    num * sizeof(T)     // Tamaño de la porción a liberar. La Liberamos toda.
                );
            };

            return { static_cast<T*>(mapped_mem), mmap_deleter };
        }

        // Obtener información sobre el estado del archivo: tamaño, permisos, usuario, etc.
        struct stat status()
        {
            if (fd_ < 0) throw std::runtime_error( "El objeto no contiene un descriptor válido." );

            struct stat statbuf;
            int return_code = fstat(fd_, &statbuf);
            if (return_code < 0)
            {
                throw std::system_error(errno, std::system_category(), "Fallo en fstat()");
            }
                
            return statbuf;
        }

        // Constructores de copia y operadores de asignación.
        //
        //  Si un objeto de C++ se puede copiar asumimos que cada copia es independiente del original, de forma que
        //  cada una se puede destruir sin afectar a las demás. Pero cuando un objeto de C++ contiene un recurso del
        //  sistema que no se puede duplicar, es mejor hacer que el objeto de C++ tampoco sea copiable, para que imite
        //  las restricciones del recurso que contiene. De lo contrario podemos tener problemas por tener.
        //
        //  Por ejemplo, si se pudiera hacer copia de objetos 'memory_map', ambas copias harían referencia al mismo
        //  archivo abierto a través del descriptor de archivo fd_. Si luego se destruye uno de los objetos, este
        //  cerrará el descriptor de archivo y el otro objeto de C++ ya no podrá utilizarlo.

        // Borrar el constructor de copia y el operador de asignación para evitar el problema comentado
        // con la copia de objetos

        memory_map(const memory_map&) = delete;
        memory_map& operator=(const memory_map&) = delete;

        // Sí podemos mover objetos, haciendo que el operador de asignación por movimiento se lleve el descriptor al
        // nuevo objeto y lo pierda el de origen.

        memory_map& operator=(memory_map&& lhs)
        {
            // Mover el descriptor de archivo al objeto asignado.
            fd_ = lhs.fd_;
            lhs.fd_ = -1;

            return *this;
        }

    private:
        int fd_ = -1;
    };
}