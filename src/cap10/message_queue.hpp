// message_queue -  Clase de colas de mensajes
//

#pragma once

#include <mqueue.h>     // Cabecera de las colas de mensajes 

#include <array>
#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <system_error>
#include <tuple>

namespace examples
{
    class message_queue
    {
    public:

        static constexpr size_t MAX_MESSAGE_SIZE = 8196;

        enum class open_mode
        {
            read_write = O_RDWR,
            read_only = O_RDONLY,
            write_only = O_WRONLY 
        };

        message_queue() {}

        message_queue(const std::string& name, open_mode mode, bool server_mode = false)
            : name_{name}
        {
            int flags = static_cast<int>(mode);

            if (server_mode)
            {
                // En 'server_mode' somos responsable de crear la cola de mensajes y de borrarla al terminar. Usamos el
                // flag O_EXCL para que de un error si ya existe una cola con el mismo nombre.
                flags |= server_mode ? O_CREAT | O_EXCL : 0;

                // Crear la cola de mensajes indicada en 'name'.
                mqd_ = mq_open( name_.c_str(), flags, 0666, nullptr );
            }
            else {
                // Abrir la cola de mensajes indicada en 'name'.
                mqd_ = mq_open( name_.c_str(), flags );
            }

            if (mqd_ < 0)
            {
                throw std::system_error( errno, std::system_category(), "Fallo en mq_open()" );
            }

            unlink_flag_ = server_mode;
        }

        // Asegurar que se liberan todos los recursos reservados en el constructor.
        ~message_queue()
        {
            if (mqd_ >= 0) 
            {
                mq_close( mqd_ );
            }

            if (unlink_flag_)
            {
                mq_unlink( name_.c_str() );
            }
        }

        // Función para recibir mensajes que hayan llegado a la cola de mensajes.
        // Devuelve el mensaje y su prioridad.
        std::tuple<std::string, unsigned int> receive()
        {
            std::array<char, MAX_MESSAGE_SIZE> buffer{};
            unsigned int prio;

            ssize_t return_code = mq_receive(mqd_, buffer.data(), buffer.size(), &prio);
            if (return_code < 0)
            {
                throw std::system_error( errno, std::system_category(), "Fallo en mq_receive()" );
            }

            return {
                { buffer.data(), static_cast<size_t>(return_code) },    // Mensaje
                prio                                                    // Prioridad
            };
        }

        // Función para enviar mensajes a la cola de mensajes.
        void send(const std::string& message, unsigned int priority = 0 )
        {
            ssize_t return_code = mq_send( mqd_, message.c_str(), message.size(), priority );
            if (return_code < 0)
            {
                throw std::system_error( errno, std::system_category(), "Fallo en mq_send()" );
            }
        }

        // Constructores de copia y operadores de asignación.
        //
        //  Si un objeto de C++ se puede copiar asumimos que cada copia es independiente del original, de forma que
        //  cada una se puede destruir sin afectar a las demás. Pero cuando un objeto de C++ contiene un recurso del
        //  sistema que no se puede duplicar, es mejor hacer que el objeto de C++ tampoco sea copiable, para que imite
        //  las restricciones del recurso que contiene. De lo contrario podemos tener problemas por tener.
        //
        //  Por ejemplo, si se pudiera hacer copia de objetos 'message_queue', ambas copias harían referencia a la
        //  misma cola de mensajes a través del descriptor de archivo fd_. Si luego se destruye uno de los objetos,
        //  este cerrará el descriptor de la cola de mensajes y el otro objeto de C++ ya no podrá utilizarlo.

        // Borrar el constructor de copia y el operador de asignación para evitar el problema comentado
        // con la copia de objetos

        message_queue(const message_queue&) = delete;
        message_queue& operator=(const message_queue&) = delete;

        // Sí podemos mover objetos, haciendo que el operador de asignación por movimiento se lleve el descriptor al
        // nuevo objeto y lo pierda el de origen.

        message_queue& operator=(message_queue&& lhs)
        {
            name_ = std::move(lhs.name_);

            // Mover el descriptor de archivo al objeto asignado.
            mqd_ = lhs.mqd_;
            lhs.mqd_ = -1;

            // Mover el flag. Solo un objeto de C++ debe hacerse cargo de borrar la cola de mensajes.
            unlink_flag_ = lhs.unlink_flag_;
            lhs.unlink_flag_ = false;

            return *this;
        }

    private:
        std::string name_;
        bool unlink_flag_ = false;
        int mqd_ = -1;
    };
} // namespace examples