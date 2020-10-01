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

    shared_memory()
        : fd_{-1}
    {}

    shared_memory(const std::string name, bool create = false, size_t size = 0 )
        : name_{name}, created_{false}, size_{size}, fd_{-1}
    {
        int flags = create ? O_CREAT : 0;
        flags |= O_RDWR;

        // Abrir el objeto de memoria indicado en 'name'.
        fd_ = shm_open( name_.c_str(), flags, 666 );
        if (fd_ < 0)
        {
            throw std::system_error( errno, std::system_category(), "Fallo en shm_open()" );
        }

        created_ = create;

        if (size_ > 0)
        {
            // Extender el objeto a la longitud indicada en 'size'
            int return_code = ftruncate( fd_, size_ );
            if (return_code < 0)
            {
                throw std::system_error( errno, std::system_category(), "Fallo en ftruncate()" );
            }
        }
    }

    // Los objetos shared_memory envuelven un recurso no duplicable fácilmente, el descriptor de
    // archivo del objeto de memoria compartida abierto en fd_. Por eso estas clases no deben ser
    // copiables. De lo contrario cada copia realmente hará referencia al mismo descriptor de
    // archivo y eso puede dar problemas si una de las copias los cierra.

    shared_memory(const shared_memory&) = delete;
    shared_memory& operator=(const shared_memory&) = delete;

    // Pero sí podemos mover objetos, haciendo que el asignado por movimiento se lleve el
    // descriptor y lo pierda el de origen.

    shared_memory& operator=(shared_memory&& lhs)
    {
        name_ = std::move(lhs.name_);
        size_ = lhs.size_;

        // Mover el descriptor de archivo al objeto asignado.
        fd_ = lhs.fd_;
        lhs.fd_ = -1;

        return *this;
    }

    ~shared_memory()
    {
        if (fd_ >= 0) 
        {
            close( fd_ );
        }

        if (created_)
        {
            shm_unlink( name_.c_str() );
        }
    }

    // Para asegurar que los punteros a regiones de memoria mapeadas se liberan automáticamente
    // cuando no las necesitamos, lo mejor es utilizar punteros inteligentes.

    template<typename T>
    std::unique_ptr<T, std::function<void(T*)>> map( int prot, size_t length = 0, off_t offset = 0 )
    {
        if (fd_ < 0) throw std::runtime_error( "El objeto no contiene un descriptor válido." );

        length = length ? length : sizeof(T);
        void* shared_mem = mmap( NULL, length, prot, MAP_SHARED, fd_, offset );
        if (shared_mem == MAP_FAILED)
        {
            throw std::system_error( errno, std::system_category(), "Fallo en mmap()" );
        }

        auto mmap_deleter = [length](T* addr)
        {
            munmap( addr, length );
        };

        return { static_cast<T*>(shared_mem),  mmap_deleter };
    }

private:
    std::string name_;
    bool created_;
    size_t size_;
    int fd_;
};