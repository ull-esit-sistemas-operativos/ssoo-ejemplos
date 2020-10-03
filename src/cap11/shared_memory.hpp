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
#include <memory>
#include <stdexcept>
#include <system_error>

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
        fd_ = shm_open( name_.c_str(), flags, 666 );
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

    // Para reservar una región de la memoria virtual del proceso y mapear en ella el objeto de memoria compartida,
    // se usa mmap(). Mientras que para liberarlo munmap().
    // En C++ no es adecuado depender de "acordamos de liberarla" cuando ya no la necesitamos. En su lugar, debemos
    // usar punteros inteligentes (como std::unique_ptr) para que se llame automáticamente a munmap() cuando ya no
    // la vayamos a necesitar más.

    // Esta función mapea num objetos de tipo T, es decir, sizeof(T) * num bytes. Así se puede acceder a un solo
    // elemento T o un array de T en la memoria compartida.

    template <typename T>
    std::unique_ptr<T, std::function<void(T*)>> map( int prot, size_t num = 1, off_t offset = 0 )
    {
        if (fd_ < 0) throw std::runtime_error( "El objeto no contiene un descriptor válido." );
 
        void* shared_mem = mmap( nullptr, num * sizeof(T), prot, MAP_SHARED, fd_, offset );
        if (shared_mem == MAP_FAILED)
        {
            throw std::system_error( errno, std::system_category(), "Fallo en mmap()" );
        }

        auto mmap_deleter = [num](T* addr)
        {
            munmap(
                addr,               // Puntero a la región a liberar (devuelto por mmap())
                num * sizeof(T)     // Tamaño de la porción a liberar. La Liberamos toda.
            );
        };

        return { static_cast<T*>(shared_mem),  mmap_deleter };
    }

    // Si un objeto de C++ se puede copiar es asumimos que la copia es independiente del original, que se puede
    // destruir sin problemas. Pero cuando un objeto de C++ contiene un recurso del sistema que no se puede duplicar,
    // es mejor hacer que el objeto de C++ tampoco sea copiable, para que imite las restricciones del recurso que
    // abstrae. De lo contrario podemos tener problemas por tener, por ejemplo, dos objetos de C++ que hacen referencia
    // al mismo objeto de memoria compartida; porque si uno de ellos es destruido, destruirá el recurso compartido
    // por ambos.

    // Borrar el constructor de copia y el operador de asignación para evitar el clonado del objeto.

    shared_memory(const shared_memory&) = delete;
    shared_memory& operator=(const shared_memory&) = delete;

    // Sí podemos mover objetos, haciendo que el operador de asignación por movimiento se lleve el descriptor al nuevo
    // objeto y lo pierda el de origen.

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