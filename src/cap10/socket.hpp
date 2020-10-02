// socket.hpp -  Clase de sockets no orientados a conexión
//

#pragma once

#include <unistd.h>
#include <sys/socket.h> // Cabecera de sockets
#include <sys/types.h>
#include <sys/un.h>     // Cabecera de sockets de dominio UNIX

#include <array>
#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <system_error>

const std::string SOCKET_UNNAMED;

class socket_t
{
public:

    socket_t() {}

    socket_t(const std::string& pathname)
        : pathname_{pathname}
    {
        // Crear el socket de dominio UNIX
        sockfd_ = socket( AF_UNIX, SOCK_DGRAM, 0 );
        if (sockfd_ < 0)
        {
            throw std::system_error( errno, std::system_category(), "Fallo en socket()" );
        }

        // Asignar la 'pathname' como dirección del socket creado.
        if (! pathname.empty())
        {
            created_ = true;

            sockaddr_un address = make_address( pathname );

            int return_code = bind( sockfd_, reinterpret_cast<sockaddr*>(&address),
                sizeof(address) );
            if (return_code < 0)
            {
                throw std::system_error( errno, std::system_category(), "Fallo en bind()" );
            }
        }
 }

    // Los objetos socket_t envuelven un recurso no duplicable fácilmente, el descriptor del socket
    // abierto en sockfd_. Por eso estas clases no deben ser copiables. De lo contrario cada copia
    // realmente hará referencia al mismo descriptor y eso puede dar problemas si una de las copias
    // lo cierra.

    socket_t(const socket_t&) = delete;
    socket_t& operator=(const socket_t&) = delete;

    // Pero sí podemos mover objetos, haciendo que el operador de asignación por movimiento se
    // lleve el descriptor y lo pierda el de origen.

    socket_t& operator=(socket_t&& lhs)
    {
        pathname_ = std::move(lhs.pathname_);

        // Mover el descriptor del socket al objeto destino.
        sockfd_ = lhs.sockfd_;
        lhs.sockfd_ = -1;

        created_ = lhs.created_;
        lhs.created_ = false;

        return *this;
    }

    ~socket_t()
    {
        if (sockfd_ >= 0) 
        {
            close( sockfd_ );
        }

        if (created_)
        {
            unlink( pathname_.c_str() );
        }
    }

    std::string receive()
    {
        std::array<char, 8196> buffer;
        ssize_t return_code = recvfrom(sockfd_, buffer.data(), buffer.size(), 0, nullptr, nullptr);
        if (return_code < 0)
        {
            throw std::system_error( errno, std::system_category(), "Fallo en recvfrom()" );
        }

        return { buffer.data(), static_cast<size_t>(return_code) };
    }

    void send(const std::string& message, const std::string& destination_name )
    {
        sockaddr_un address = make_address( destination_name );
        ssize_t return_code = sendto( sockfd_, message.c_str(), message.size(), 0,
            reinterpret_cast<sockaddr*>(&address), sizeof(address) );
        if (return_code < 0)
        {
            throw std::system_error( errno, std::system_category(), "Fallo en sendto()" );
        }
    }

private:
    int sockfd_ = -1;
    bool created_ = false;
    std::string pathname_;

    sockaddr_un make_address(const std::string& name)
    {
        sockaddr_un address = {};
        address.sun_family = AF_UNIX;
        name.copy( address.sun_path, sizeof(address.sun_path) - 1, 0 );

        return address;
    }
};