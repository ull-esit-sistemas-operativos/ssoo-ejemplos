# Colas de mensajes POSIX

Las [colas de mensajes POSIX](https://man7.org/linux/man-pages/man7/mq_overview.7.html) son un mecanismo de comunicación entre procesos que permite enviar y recibir mensajes de tamaño variable, cada uno con una prioridad asociada, de forma que los mensajes de mayor prioridad se reciben antes que los de menor prioridad, independientemente del orden de envío.

Cada cola de mensajes se identifica con un nombre, similar al de una tubería con nombre, que dos o más procesos no relacionados pueden usar para abrir la misma cola y comunicarse a través de ella.

En [`message_queue.hpp`](message_queue.hpp) se puede ver un ejemplo de una clase desarrollada en C++ para utilizar colas de mensajes POSIX.
En los distintos métodos se puede ver cómo se utilizan las funciones de la librería del sistema —[`mq_open()`](https://man7.org/linux/man-pages/man3/mq_open.3.html), [`mq_send()`](https://man7.org/linux/man-pages/man3/mq_send.3.html), [`mq_receive()`](https://man7.org/linux/man-pages/man3/mq_receive.3.html), [`mq_close()`](https://man7.org/linux/man-pages/man3/mq_close.3.html) y [`mq_unlink()`](https://man7.org/linux/man-pages/man3/mq_unlink.3.html)— para crear la cola y enviar y recibir mensajes.

En [`mqueue-server.cpp`](mqueue-server.cpp) y [`mqueue-client.cpp`](mqueue-client.cpp) se ilustra cómo se utiliza la clase en [`message_queue.hpp`](message_queue.hpp).
El primero es un programa que muestra la hora del sistema periódicamente, mientras que el segundo se puede comunicar con el primero a través de una cola de mensajes para controlarlo.

El ejemplo es muy sencillo, así que, por el momento, lo único que puede hacer [`mqueue-client.cpp`](mqueue-client.cpp) es pedirle a [`mqueue-server.cpp`](mqueue-server.cpp) que termine.
Sin embargo, no costaría nada añadir otras órdenes, como pedir que cambie la hora del sistema o la periodicidad con la que la muestra.
