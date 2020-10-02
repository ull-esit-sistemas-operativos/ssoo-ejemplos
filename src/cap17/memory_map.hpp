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
#include <memory>
#include <stdexcept>
#include <system_error>

class memory_map
{
public:

    memory_map() {}

    memory_map(const std::string& pathname, int flags)
        : pathname_{pathname}
    {
        int flags_mask = O_RDWR | O_RDONLY |  O_WRONLY;
        flags &= flags_mask;

        // Abrir el archivo indicado en 'pathname'.
        fd_ = open( pathname_.c_str(), flags, 666 );
        if (fd_ < 0)
        {
            throw std::system_error( errno, std::system_category(), "Fallo en shm_open()" );
        }
    }

    // Los objetos memory_map envuelven un recurso no duplicable fácilmente, el descriptor de
    // archivo abierto en fd_. Por eso estas clases no deben ser copiables. De lo contrario cada
    // copia realmente haría referencia al mismo descriptor de archivo y eso puede dar problemas si
    // una de las copia lo cierra.

    memory_map(const memory_map&) = delete;
    memory_map& operator=(const memory_map&) = delete;

    // Pero sí podemos mover objetos, haciendo que el operador de asignación por movimiento se
    // lleve el descriptor y lo pierda el de origen.

    memory_map& operator=(memory_map&& lhs)
    {
        pathname_ = std::move(lhs.pathname_);

        // Mover el descriptor del socket al objeto destino.
        fd_ = lhs.fd_;
        lhs.fd_ = -1;

        return *this;
    }

    ~memory_map()
    {
        if (fd_ >= 0) 
        {
            close( fd_ );
        }
    }

    // Para asegurar que los punteros a regiones de memoria mapeadas se liberan automáticamente
    // cuando no las necesitamos, lo mejor es utilizar punteros inteligentes, 

    template<typename T>
    std::unique_ptr<T, std::function<void(T*)>> map( int prot, size_t length = 0, off_t offset = 0)
    {
        if (fd_ < 0) throw std::runtime_error( "El objeto no contiene un descriptor válido." );

        length = length ? length : sizeof(T);
        void* mapped_mem = mmap( nullptr, length, prot, MAP_SHARED, fd_, offset );
        if (mapped_mem == MAP_FAILED)
        {
            throw std::system_error( errno, std::system_category(), "Fallo en mmap()" );
        }

        auto mmap_deleter = [length](T* addr)
        {
            munmap( addr, length );
        };

        return { static_cast<T*>(mapped_mem), mmap_deleter };
    }

    // Obtener el tamaño del archivo.
    size_t size()
    {
        if (fd_ < 0) throw std::runtime_error( "El objeto no contiene un descriptor válido." );

        struct stat statbuf;
        int return_code = fstat(fd_, &statbuf);
        if (return_code < 0)
        {
            throw std::system_error(errno, std::system_category(), "Fallo en fstat()");
        }
            
        return statbuf.st_size;;
    }

private:
    std::string pathname_;
    int fd_ = -1;
};