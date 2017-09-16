// memory_map.h - Clase para mapear archivos en memoria con mmap()
#pragma once

#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <cerrno>
#include <system_error>

class memory_map
{
public:
    
    static const mode_t DEFAULT_OPEN_MODE = S_IRUSR | S_IWUSR | \
                                            S_IRGRP | S_IWGRP | \
                                            S_IROTH | S_IWOTH;     // rw-rw-rw-
    
    // constructor para mapear un archivo completo en memoria
    memory_map(const char* pathname, int open_flags, size_t size=0, mode_t open_mode=DEFAULT_OPEN_MODE)
        : fd_{-1}, size_{size}, data_{MAP_FAILED}
    {
        // abrir el archivo indicado en pathname        
        fd_ = open(pathname, open_flags, open_mode);
        if (fd_ < 0)
            throw std::system_error(errno, std::system_category(), "fallo en open()");

        if (size_ > 0) {
            // extender el archivo a la longitud indicada en size_
            int result = ftruncate(fd_, size_);
            if (result < 0)
                throw std::system_error(errno, std::system_category(), "fallo en ftruncate()");
        }
        else {
            // obtener el tamaño del mapeo del tamaño del archivo
            struct stat statbuf;
            int result = fstat(fd_, &statbuf);
            if (result < 0)
                throw std::system_error(errno, std::system_category(), "fallo en fstat()");
            
            size_ = statbuf.st_size;
        }

        // configurando la protección de memoria adecuada para el modo de acceso del archivo
        int mmap_prot = PROT_READ;
        if (open_flags & O_WRONLY)
            mmap_prot = PROT_WRITE;
        else if (open_flags & O_RDWR)
            mmap_prot = PROT_READ | PROT_WRITE;
        
        // mapear el archivo abierto
        data_ = mmap(NULL, size_, mmap_prot, MAP_SHARED, fd_, 0);
        if (data_ == MAP_FAILED)
            throw std::system_error(errno, std::system_category(), "fallo en mmap()");
    }
    
    // constructor para crear un mapeo no respalddo por ningún archivo
    memory_map(int mmap_prot, size_t size)
        : fd_{-1}, size_{size}, data_{MAP_FAILED}
    {
        data_ = mmap(NULL, size_, mmap_prot, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
        if (data_ == MAP_FAILED)
            throw std::system_error(errno, std::system_category(), "fallo en mmap()");
    }
    
    ~memory_map()
    {
        if (data_ != MAP_FAILED)
            munmap(data_, size_);
        
        if (fd_ > 0)
            close(fd_);
    }
    
    template <typename T>
    T* data()
    {
        return static_cast<T*>(data_);
    }
    
    size_t size()
    {
        return size_;
    }
    
private:
    int fd_;
    size_t size_;
    void* data_;
};
