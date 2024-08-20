// shared_memory.hpp -  Clase de objetos de memoria compartida
//

#pragma once

#include <unistd.h>     // Cabecera principal de la API POSIX del sistema operativo
#include <fcntl.h>
#include <sys/mman.h>   // Cabecera para mmap() y shm_open()
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
    class shared_memory
    {
    public:

        shared_memory() {}

        shared_memory(const std::string& name, bool server_mode = false, size_t size = -1)
            : name_{name}, size_{size}
        {
            // En 'server_mode' somos responsable de crear el objeto de memoria compartida y de borrarlo al terminar.
            // Usamos el flag O_EXCL para que de un error si ya existe un objeto con el mismo nombre.
            int flags = O_RDWR | (server_mode ? O_CREAT | O_EXCL : 0);

            // Abrir el objeto de memoria indicado en 'name'.
            fd_ = shm_open( name_.c_str(), flags, 0666 );
            if (fd_ < 0)
            {
                throw std::system_error( errno, std::system_category(), "Fallo en shm_open()" );
            }

            // Recordar que somos los responsables de eliminar el objeto de memoria compartida para cuando llegue el
            // momento de destruir el objeto de C++.
            unlink_flag_ = server_mode;

            // Si el objeto se ha creado, su tamaño es 0. Extenderlo a la longitud indicada en 'size'.
            if (server_mode && size_ > 0)
            {
                int return_code = ftruncate( fd_, size_ );
                if (return_code < 0)
                {
                    throw std::system_error( errno, std::system_category(), "Fallo en ftruncate()" );
                }
            }
        }

        // Asegurar que se liberan todos los recursos reservados en el constructor.
        ~shared_memory()
        {
            if (fd_ >= 0) 
            {
                close( fd_ );
            }

            if (unlink_flag_)
            {
                shm_unlink( name_.c_str() );
            }
        }

        // map() - Mapear en la memoria del proceso 'num' objetos de tipo T (es decir, 'sizeof(T) * num' bytes) del
        //  objeto de memoria compartida abierto en 'fd_'. Así se puede mapear en la memoria compartida a un solo
        //  elemento T o un array de elementos T.
        //
        // NOTAS:
        //  Se usa mmap() para reservar una región de la memoria virtual del proceso y mapear en ella el objeto de
        //  memoria compartida. Mientras que para liberar la región reservada se usa munmap().

        //  En C++ no es adecuado depender de "acordamos de llamar a munmap()" cuando ya no necesitamos la memoria
        //  mapeada. En su lugar, debemos usar punteros inteligentes (como std::unique_ptr) para que se llame
        //  automáticamente a munmap() cuando el puntero va a ser destruido.

        template <typename T>
        using mmap_deleter_func = std::function<void(T*)>;

        template <typename T>
        std::unique_ptr<T, mmap_deleter_func<T>> map( int prot, size_t num = 1, off_t offset = 0 )
        {
            if (fd_ < 0) throw std::runtime_error( "El objeto no contiene un descriptor válido." );
    
            void* shared_mem = mmap( nullptr, num * sizeof(T), prot, MAP_SHARED, fd_, offset );
            if (shared_mem == MAP_FAILED)
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

            return { static_cast<T*>(shared_mem),  mmap_deleter };
        }

        // Constructores de copia y operadores de asignación.
        //
        //  Si un objeto de C++ se puede copiar asumimos que cada copia es independiente del original, de forma que
        //  cada una se puede destruir sin afectar a las demás. Pero cuando un objeto de C++ contiene un recurso del
        //  sistema que no se puede duplicar, es mejor hacer que el objeto de C++ tampoco sea copiable, para que imite
        //  las restricciones del recurso que contiene. De lo contrario podemos tener problemas por tener.
        //
        //  Por ejemplo, si se pudiera hacer copia de objetos 'shared_memory', ambas copias harían referencia al mismo
        //  objeto de memoria compartida a través del descriptor de archivo fd_. Si luego se destruye uno de los
        //  objetos, este cerrará el descriptor de archivo y el otro objeto de C++ ya no podrá utilizarlo.

        // Borrar el constructor de copia y el operador de asignación para evitar el problema comentado
        // con la copia de objetos

        shared_memory(const shared_memory&) = delete;
        shared_memory& operator=(const shared_memory&) = delete;

        // Sí podemos mover objetos, haciendo que el operador de asignación por movimiento se lleve el descriptor al
        // nuevo objeto y lo pierda el de origen.

        shared_memory& operator=(shared_memory&& lhs)
        {
            name_ = std::move(lhs.name_);
            size_ = lhs.size_;

            // Mover el descriptor de archivo al objeto asignado.
            fd_ = lhs.fd_;
            lhs.fd_ = -1;

            // Mover el flag. Solo un objeto de C++ debe hacerse cargo de borrar el objeto de memoria compartida.
            unlink_flag_ = lhs.unlink_flag_;
            lhs.unlink_flag_ = false;

            return *this;
        }

    private:

        std::string name_;
        bool unlink_flag_ = false;
        size_t size_;
        int fd_ = -1;
    };
} // namespace examples