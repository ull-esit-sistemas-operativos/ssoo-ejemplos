// message_queue -  Clase de colas de mensajes
//

#pragma once

#include <mqueue.h>     // Cabecera de las colas de mensajes 

#include <array>
#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <system_error>

class message_queue
{
public:

    message_queue() {}

    message_queue(const std::string& name, int flags, bool create = false)
        : name_{name}
    {
        int flags_mask = O_RDWR | O_RDONLY |  O_WRONLY;
        flags &= flags_mask;
        flags |= create ? O_CREAT : 0;

        // Abrir la cola de mensajes indicada en 'name'.
        mqd_ = mq_open( name_.c_str(), flags, 666, nullptr );
        if (mqd_ < 0)
        {
            throw std::system_error( errno, std::system_category(), "Fallo en mqueue_open()" );
        }

        created_ = create;
    }

    // Los objetos message_queue envuelven un recurso no duplicable fácilmente, el descriptor de
    // de la cola de mensajes abierto en mqd_. Por eso estas clases no deben ser copiables.
    // De lo contrario cada copia realmente haría referencia al mismo descriptor y eso puede dar
    // problemas si una de las copias lo cierra.

    message_queue(const message_queue&) = delete;
    message_queue& operator=(const message_queue&) = delete;

    // Pero sí podemos mover objetos, haciendo que el operador de asignación por movimiento se
    // lleve el descriptor y lo pierda el de origen.

    message_queue& operator=(message_queue&& lhs)
    {
        name_ = std::move(lhs.name_);

        // Mover el descriptor de archivo al objeto asignado.
        mqd_ = lhs.mqd_;
        lhs.mqd_ = -1;

        created_ = lhs.created_;
        lhs.created_ = false;

        return *this;
    }

    ~message_queue()
    {
        if (mqd_ >= 0) 
        {
            mq_close( mqd_ );
            if (created_)
            {
                mq_unlink( name_.c_str() );
            }
        }
    }

    std::string receive()
    {
        std::array<char, 8196> buffer;
        ssize_t return_code = mq_receive(mqd_, buffer.data(), buffer.size(), nullptr);
        if (return_code < 0)
        {
            throw std::system_error( errno, std::system_category(), "Fallo en mq_receive()" );
        }

        return { buffer.data(), static_cast<size_t>(return_code) };
    }

    void send(const std::string& message, unsigned int priority = 0 )
    {
        ssize_t return_code = mq_send( mqd_, message.c_str(), message.size(), priority );
        if (return_code < 0)
        {
            throw std::system_error( errno, std::system_category(), "Fallo en mqueue_send()" );
        }
    }

private:
    std::string name_;
    bool created_ = false;
    int mqd_ = -1;
};