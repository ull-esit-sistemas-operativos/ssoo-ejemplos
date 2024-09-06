// socket.hpp -  Clase de sockets no orientados a conexión
//

#pragma once

#include <array>
#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <system_error>
#include <tuple>

#include <unistd.h>
#include <sys/socket.h> // Cabecera de sockets
#include <sys/types.h>
#include <sys/un.h>     // Cabecera de sockets de dominio UNIX

namespace examples
{
    class socket
    {
    public:

        static constexpr char* SOCKET_UNNAMED = nullptr; 
        static constexpr size_t MAX_MESSAGE_SIZE = 8196;

        socket() {}

        socket(const char* pathname)
        {
            // Crear el socket de dominio UNIX no orientados a conexión
            sockfd_ = ::socket( AF_UNIX, SOCK_DGRAM, 0 );
            if (sockfd_ < 0)
            {
                throw std::system_error( errno, std::system_category(), "Fallo en socket()" );
            }

            // Asignar 'pathname' como dirección del socket local creado.
            if (pathname)
            {
                pathname_ = pathname;

                // Construir la dirección en el formato que necesita bind().
                sockaddr_un local_address = make_address( pathname_ );

                int return_code = bind( sockfd_, reinterpret_cast<sockaddr*>(&local_address), sizeof(local_address) );
                if (return_code < 0)
                {
                    throw std::system_error( errno, std::system_category(), "Fallo en bind()" );
                }

                // Recordar que somos los responsables de eliminar el archivo especial que representa a los socket de
                // dominio UNIX en el sistema de archivos cuando llegue el momento de destruir el objeto de C++.
                unlink_flag_ = true;
            }
        }

        socket(const std::string& pathname)
            : socket{ pathname.c_str() }
        {}

        // Asegurar que se liberan todos los recursos reservados en el constructor.
        ~socket()
        {
            if (sockfd_ >= 0) 
            {
                close( sockfd_ );
            }

            if (unlink_flag_)
            {
                unlink( pathname_.c_str() );
            }
        }

        // Función para recibir mensajes que hayan llegado al socket.
        // Devuelve el mensaje y la dirección del remitente.
        std::tuple<std::string, sockaddr_un> receive()
        {
            std::array<char, MAX_MESSAGE_SIZE> buffer{};
            
            sockaddr_un remote_address;
            socklen_t address_length = sizeof(remote_address);

            // Al volver de recvfrom, si todo ha ido bien, 'buffer' contiene el mensaje, 'remote_address' la dirección del
            // socket del remitente y 'address_length' el tamaño de la estructura copiada en 'remote_address';       
            ssize_t return_code = recvfrom(sockfd_, buffer.data(), buffer.size(), 0,
                reinterpret_cast<sockaddr*>(&remote_address), &address_length);
            if (return_code < 0)
            {
                throw std::system_error( errno, std::system_category(), "Fallo en recvfrom()" );
            }

            return {
                { buffer.data(), static_cast<size_t>(return_code) },    // Mensaje
                remote_address                                          // Dirección remitente
            };
        }

        // Función para enviar mensajes desde el socket al socket de destino indicado.
        void send(const std::string& message, const std::string& destination )
        {
            // Construir la dirección en el formato que necesita sendto().
            sockaddr_un remote_address = make_address( destination );

            ssize_t return_code = sendto( sockfd_, message.c_str(), message.size(), 0,
                reinterpret_cast<sockaddr*>(&remote_address), sizeof(remote_address) );
            if (return_code < 0)
            {
                throw std::system_error( errno, std::system_category(), "Fallo en sendto()" );
            }
        }

        // Constructores de copia y operadores de asignación.
        //
        //  Si un objeto de C++ se puede copiar asumimos que cada copia es independiente del original, de forma que
        //  cada una se puede destruir sin afectar a las demás. Pero cuando un objeto de C++ contiene un recurso del
        //  sistema que no se puede duplicar, es mejor hacer que el objeto de C++ tampoco sea copiable, para que imite
        //  las restricciones del recurso que contiene. De lo contrario podemos tener problemas por tener.
        //
        //  Por ejemplo, si se pudiera hacer copia de objetos 'examples::socket', ambas copias harían referencia al
        //  mismo socket del sistema a través del descriptor de archivo fd_. Si luego se destruye uno de los objetos,
        //  este cerrará el descriptor del socket y el otro objeto de C++ ya no podrá utilizarlo.

        // Borrar el constructor de copia y el operador de asignación para evitar el problema comentado
        // con la copia de objetos

        socket(const socket&) = delete;
        socket& operator=(const socket&) = delete;

        // Sí podemos mover objetos, haciendo que el operador de asignación por movimiento se lleve el descriptor al nuevo
        // objeto y lo pierda el de origen.

        socket& operator=(socket&& lhs)
        {
            pathname_ = std::move(lhs.pathname_);

            // Mover el descriptor del socket al objeto destino.
            sockfd_ = lhs.sockfd_;
            lhs.sockfd_ = -1;

            unlink_flag_ = lhs.unlink_flag_;
            lhs.unlink_flag_ = false;

            return *this;
        }

    private:
        int sockfd_ = -1;
        bool unlink_flag_ = false;
        std::string pathname_;

        // Función para construir direcciones en el formato que necesita la librería de sockets
        sockaddr_un make_address(const std::string& pathname)
        {
            sockaddr_un address = {};
            address.sun_family = AF_UNIX;
            pathname.copy( address.sun_path, sizeof(address.sun_path) - 1, 0 );

            return address;
        }
    };
} // namespace examples