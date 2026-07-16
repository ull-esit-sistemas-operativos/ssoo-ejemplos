# Sockets

Un _socket_ es una abstracción que representa un extremo de un canal de comunicación entre dos procesos en una red de ordenadores.
Cada proceso puede utilizar su _socket_ para recibir mensajes de otros procesos o para enviarlos a través de dicho canal de comunicación, sin tener que entrar en los detalles de cómo se usa el protocolo de red escogido, ni cómo se gestionan los dispositivos de red involucrados en la comunicación.
Estos aspectos son responsabilidad del sistema operativo y del hardware de la red, pero no del programador de las aplicaciones.

**Tabla de contenidos**
- [Crear _sockets_](#crear-sockets)
- [Cerrar descriptores de _sockets_](#cerrar-descriptores-de-sockets)
- [Asignar una dirección al socket {#sec-bind}](#asignar-una-dirección-al-socket-sec-bind)
  - [Direcciones `AF_INET`](#direcciones-af_inet)
  - [Direcciones `AF_UNIX`](#direcciones-af_unix)
- [Enviar un mensaje](#enviar-un-mensaje)
- [Recibir un mensaje](#recibir-un-mensaje)
- [_Sockets_ `SOCK_STREAM`](#sockets-sock_stream)
  - [Escuchar conexiones](#escuchar-conexiones)
  - [Aceptar conexiones](#aceptar-conexiones)
  - [Envío y recepción de mensajes](#envío-y-recepción-de-mensajes)

## Crear _sockets_

Un _socket_ se crea con la función de la librería del sistema [`socket()`](https://manpages.debian.org/stretch/manpages-es/socket.2.es.html) y devuelve un descriptor de archivo, con el que identificar al _socket_ en cuestión en futuras operaciones que queramos hacer con él.

```c
int fd = socket(domain, type, protocol);
```

El significado de los parámetros de la función es el siguiente:

* `domain` es el dominio o familia de la dirección.
Indica el tipo de tecnología de comunicación que queremos que utilice nuestro *socket*.
Valores posibles son:
  * `AF_INET`, si queremos usar TCP/IP
  * `AF_INET6`, si queremos usar TCP/IPv6
  * `AF_UNIX`, si queremos usar un tipo de _socket_ local a la máquina, similar a las tuberías.

* `type` indica el tipo de _socket_ que nos interesa según los requisitos de la aplicación.
Valores posibles son:
  * `SOCK_STREAM`, si `domain` es `AF_INET`, indica que el _socket_ es para comunicaciones orientadas a conexión con TCP
  * `SOCK_DGRAM`, si `domain` es `AF_INET`, indica que queremos UDP.
  * `SOCK_RAW`, indica acceso directo a los paquetes IP.

* `protocol` indica el protocolo específico que queremos que sea utilizado internamente por el _socket_ para el envío de los datos.
    Esto es útil con algunas familias o dominios que soportan varios protocolos para cada tipo de _socket_.

    La familia IP (`AF_INET`) tanto con _sockets_ tipo `SOCK_DGRAM` como `SOCK_STREAM` solo admite UDP o TCP como protocolo, respectivamente, por lo que este argumento debe ser 0.
    Igualmente, para la familia `AF_UNIX` solo se puede usar el protocolo 0.

Con todo esto, crear un _socket_ TCP para Internet (TCP/IP) se haría de la siguiente forma:

```cpp
int fd = socket(AF_INET, SOCK_STREAM, 0);
if (fd < 0)
{
    // Error al crear el socket...
}
```

## Cerrar descriptores de _sockets_

Los descriptores de archivo de _sockets_ se cierran usando la función [`close()`](https://manpages.debian.org/stretch/manpages-es/close.2.es.html).

El motivo de error más común al llamar a `close()` es intentar cerrar un descriptor inválido o que ya ha sido cerrado.

## Asignar una dirección al socket {#sec-bind}

Para poder mandar un mensaje al _socket_ de otro proceso, el _socket_ destinatario debe tener una dirección única en la red.
Por ejemplo, en la tecnología TCP/IP (`AF_INET`) la dirección de un _socket_ se indica por la dirección IP de la máquina y un número de puerto entre 1 y 65535.

>[!NOTE]
>Si no se asigna un puerto y una dirección IP a un _socket_ y se usa para enviar mensajes, el sistema operativo escogerá un puerto libre y una de las direcciones IP de la máquina local.
>Sin embargo, para que un _socket_ pueda recibir mensajes, es necesario que primero se le asigne un puerto y una dirección IP, que también debe ser conocida por los procesos que quieren enviar mensajes a este destinatario.

Una dirección se asigna a un _socket_ que acabamos de crear mediante la función [`bind()`](https://manpages.debian.org/stretch/manpages-es/bind.2.es.html) de la librería del sistema.

```c
int bind(int fd, const sockaddr* addr, socklen_t addrlen);
```

El primer argumento `fd` de la función es el descriptor de archivo del _socket_ previamente creado con la función `socket()`, mientras que `addr` y `addrlen` indican la dirección que queremos asignar al _socket_.

La interfaz de _socket_ fue diseñada con una interfaz genérica que debía dar cabida a todas las tecnologías de comunicación existentes y futuras.
Por eso `bind()` no acepta directamente una dirección IP y un número de puerto como parámetros.
En su lugar, recibe un puntero a una estructura `sockaddr` genérica que pueda dar cabida a cualquier tipo de dirección de cualquier tecnología existente.

Según la familia de protocolos con la que queramos trabajar, se usa una versión de `sockaddr` adecuada para las direcciones de red de esa familia.
Por ejemplo, si queremos trabajar con la familia `AF_INET` tendremos que utilizar la estructura [`sockaddr_in`](https://manpages.debian.org/stretch/manpages-es/ip.7.es.html), declarada en `netinet/ip.h`, para crear las direcciones:
Mientras que si queremos trabajar con la familia `AF_UNIX` tendremos que utilizar la estructura [`sockaddr_un`](https://manpages.debian.org/stretch/manpages-es/unix.7.es.html), declarada en `sys/un.h`.

### Direcciones `AF_INET`

La estructura `sockaddr_in` para _sockets_ `AF_INET` se declara de la siguiente forma:

```c
struct sockaddr_in {
    sa_family_t     sin_family; // = AF_INET
    in_port_t       sin_port;	// Puerto del socket
    struct in_addr  sin_addr;   // Estructura con la dirección IP
};
```

Donde:

* `sin_family` es el dominio de la dirección y debe valer `AF_INET`.
* `sin_port` es el número de puerto, que debe estar en el rango de 1 a 65535.
    * En arquitecturas x86, el número de puerto debe pasar por la función [`htons()`](https://manpages.debian.org/stretch/manpages-es/htons.3.es.html) y asignar a `sin_port` el valor devuelto.
  * Si se indica 0, el sistema operativo asignará un puerto libre cualquiera.
* `sin_addr` es una estructura de tipo `in_addr` que contiene la dirección IP del _socket_.

La estructura `in_addr` del campo `sin_addr` se declara como:

```c
struct in_addr {
    uint32_t        s_addr;
};
```

Donde `s_addr` es la dirección IP en formato entero de 32 bits:

* Se puede utilizar la función [inet_aton()](https://manpages.debian.org/stretch/manpages-es/inet_aton.3.es.html) para convertir cadenas con direcciones de la forma '192.168.200.5' en estos enteros de 32 bits que necesita la estructura.
Esta función retorna 0 si la cadena no contiene una dirección IP válida.
* Si se indica `INADDR_LOOPBACK`, es para asignar la dirección 127.0.0.1 de la [interfaz de red _loopback_](https://es.wikipedia.org/wiki/Loopback) de la máquina local.
* Si se indica `INADDR_ANY` —o dirección 0.0.0.0— es porque queremos que el _socket_ esté abierto en todas las direcciones IP de la máquina local.

Por ejemplo, para asignar dirección a un _socket_ de forma que escuche en todas las direcciones IP de la máquina en un puerto escogido por el sistema operativo:

```cpp
sockaddr_in local_address{};                            // <1>
local_address.sin_family = AF_INET;                     // <2>
local_address.sin_addr.s_addr = htonl(INADDR_ANY);      // <3>
local_address.sin_port = htons(0);                      // <4>

int result = bind(
    sock_fd,                                            // <5>
    reinterpret_cast<const sockaddr*>(&local_address),  // <6>
	sizeof(local_address)                               // <7>
)
if (result < 0)
{
    // Error al asignar una dirección...
}
```
1. Inicializar a cero la dirección antes de asignarle valores, tal y como se recomienda.
Si programamos en C, se puede utilizar `memset()` para poner todos los bytes de la estructura a 0.
2. Se asigna `AF_INET` a `sin_family` porque el _socket_ es de ese dominio, así como la estructura `sockaddr_in` que se está utilizando.
3. Usar cualquier dirección IP local del equipo.
4. Asignar un puerto libre cualquiera.
En este caso, como el número es 0, es indiferente usar `htons()` o simplemente asignar un 0.
5. Descriptor del _socket_ al que `bind` debe asignar la dirección `local_address`.
6. Conversión necesaria porque `bind()` espera un puntero al formato genérico de direcciones `sockaddr`, pero `local_address` es `sockaddr_in`.
3. Tamaño de la estructura `sockaddr_in` que contiene la dirección.

### Direcciones `AF_UNIX`

La estructura `sockaddr_un` para _sockets_ `AF_UNIX` se declara de la siguiente forma:

```c
struct sockaddr_un {
    sa_family_t     sun_family;     // = AF_UNIX
    char            sun_path[108];  // Ruta del socket
};
```

Donde:

* `sun_family` es el dominio de la dirección y debe valer `AF_UNIX`.
* `sun_path` es la ruta del _socket_ en el sistema de archivos.

Por tanto, para asignar dirección a un _socket_ de forma que escuche en un _socket_ local en el sistema de archivos:

```cpp
sockaddr_un local_address{};                            // <1>
local_address.sun_family = AF_UNIX;                     // <2>
std::strcpy(local_address.sun_path, "/tmp/my_socket");  // <3>

int result = bind(
    sock_fd,                                            // <4>
    reinterpret_cast<const sockaddr*>(&local_address),  // <5>
    sizeof(local_address)                               // <6>
)
if (result < 0)
{
    // Error al asignar una dirección...
}
```
1. Inicializar a cero la dirección antes de asignarle valores, tal y como se recomienda.
2. Se asigna `AF_UNIX` a `sun_family` porque el _socket_ es de ese dominio, así como la estructura `sockaddr_un` que se está utilizando.
3. Ruta del _socket_ en el sistema de archivos.
4. Descriptor del _socket_ al que `bind` debe asignar la dirección `local_address`.
5. Conversión necesaria porque `bind()` espera un puntero al formato genérico de direcciones `sockaddr`, pero `local_address` es `sockaddr_un`.
6. Tamaño de la estructura `sockaddr_un` que contiene la dirección.

En el archivo [`sockets.cpp`](src/sockets.cpp) se muestra un ejemplo completo de cómo crear un _socket_ de dominio UNIX y asignarle una dirección.

## Enviar un mensaje

Enviar un mensaje al _socket_ de otro proceso desde el _socket_ local se puede hacer con la función [`sendto()`](https://manpages.debian.org/stretch/manpages-es/sendto.2.es.html):

```c
int sendto(int sock_fd, const void* buffer, size_t length, int flags, const sockaddr* dest_addr, socklen_t dest_len);
```

Los argumentos de la función `sendto()` son los siguientes:

- `sock_fd` es el descriptor del _socket_ local desde el que se enviará el mensaje.
- `buffer` es un puntero a la zona de memoria que contiene los datos que se quieren enviar.
- `length` es el número de bytes que se quieren enviar de la zona de memoria apuntada por `buffer`.
- `flags` son indicadores para controlar el comportamiento de la función.
  En la mayoría de los casos, se puede indicar 0.
- `dest_addr` es un puntero a la dirección del destinatario del mensaje.
- `dest_len` es el tamaño de la estructura a la que apunta `dest_addr`.

Por lo tanto, así es como se enviaría un mensaje a otro _socket_

```cpp
// Preparar la dirección del socket de destino
sockaddr_in dest_address{};
dest_address.sin_family = AF_INET;
inet_aton("192.168.12.34", &dest_address.sin_addr);
dest_address.sin_port = htons(12345);

// Preparar el mensaje
std::string message_text("¡Hola, mundo!");

// Enviar el mensaje
int bytes_sent = sendto(sock_fd, message_text.data(), message_text.size(), 0,
    reinterpret_cast<const sockaddr*>(&dest_address), sizeof(dest_address));
if (bytes_sent < 0)
{
    // Error al enviar el mensaje.
}
```

## Recibir un mensaje

Con la función [`recvfrom()`](https://manpages.debian.org/stretch/manpages-es/recvfrom.2.es.html) se puede recibir un mensaje en nuestro _socket_ enviado desde otro proceso:

```c
int recvfrom(int sock_fd, void* buffer, size_t length, int flags, sockaddr* src_addr, socklen_t* src_len);
```

Los argumentos de la función `recvfrom()` son los siguientes:

- `sock_fd` es el descriptor del _socket_ local donde se recibirá el mensaje.
- `buffer` es un puntero a la zona de memoria donde se guardarán los datos recibidos.
- `length` es el número máximo de bytes que caben en el buffer.
- `flags` son indicadores para controlar el comportamiento de la función.
  En la mayoría de los casos, se puede indicar 0.
- `src_addr` es un puntero donde se guardará la dirección del emisor del mensaje al retornar de la función.
Si no se necesita la dirección del emisor, se puede pasar `NULL`.
- `src_len` es un puntero donde se guardará el tamaño de la estructura copiada en `src_addr`.
Por ejemplo, si se están usando direcciones `sockaddr_in`, debería valer siempre `sizeof(sockaddr_in)` a la vuelta de la función.

Por lo tanto, así es como podríamos recibir un mensaje de otro proceso:

```cpp
// Variables para guardar la dirección del emisor
sockaddr_in source_address{};
socklen_t source_address_length;

// Buffer para guardar el mensaje
std::string message_text();
message_text.resize(100);

// Recibir el mensaje
int bytes_read = recvfrom(sock_fd, message_text.data(), message_text.size(), 0,
    reinterpret_cast<sockaddr*>(&source_address), &source_address_length);
if (bytes_read < 0)
{
    // Error al recibir el mensaje...
}

// Ajustar el tamaño del std::string al número real de bytes leídos
message_text.resize(bytes_read);

// Imprimir el mensaje y la dirección del remitente
std::println("El sistema '{}' envió el mensaje '{}'",
    inet_ntoa(source_address.sin_addr), message_text);
```

## _Sockets_ `SOCK_STREAM`

Los _sockets_ `SOCK_STREAM` son _sockets_ orientados a conexión, que garantizan la entrega de los mensajes en el orden en que se enviaron y sin errores.
Por tanto, se necesitan algunas funciones adicionales para establecer la conexión entre dos _sockets_ `SOCK_STREAM` antes de poder enviar y recibir mensajes.

### Escuchar conexiones

Para que un _socket_ `SOCK_STREAM` pueda recibir conexiones de otros _sockets_ `SOCK_STREAM`, primero debe ponerse en modo de escucha con la función [`listen()`](https://manpages.debian.org/stretch/manpages-es/listen.2.es.html):

```c
int listen(int sock_fd, int backlog);
```

Los argumentos de la función `listen()` son los siguientes:

- `sock_fd` es el descriptor del _socket_ que se quiere poner en modo de escucha.
- `backlog` es el número máximo de conexiones pendientes que se permiten en la cola de conexiones.
  Si se intenta conectar un nuevo _socket_ cuando la cola está llena, la conexión se rechaza.

Por ejemplo, para poner un _socket_ en modo de escucha con una cola de 5 conexiones pendientes:
    
```cpp
int result = listen(sock_fd, 5);
if (result < 0)
{
    // Error al poner el socket en modo de escucha...
}
```

### Aceptar conexiones

Una vez que un _socket_ `SOCK_STREAM` está en modo de escucha, se pueden aceptar conexiones entrantes con la función [`accept()`](https://manpages.debian.org/stretch/manpages-es/accept.2.es.html):

```c
int accept(int sock_fd, sockaddr* addr, socklen_t* addrlen);
```

Los argumentos de la función `accept()` son los siguientes:

- `sock_fd` es el descriptor del _socket_ que está en modo de escucha.
- `addr` es un puntero donde se guardará la dirección del _socket_ que se conectó.
- `addrlen` es un puntero donde se guardará el tamaño de la estructura copiada en `addr`.
Por ejemplo, si se están usando direcciones `sockaddr_in`, debería valer siempre `sizeof(sockaddr_in)` a la vuelta de la función.

La función `accept()` devuelve un nuevo descriptor de archivo que representa la conexión establecida con el _socket_ que se ha conectado.
Este descriptor se puede usar para enviar y recibir mensajes con el _socket_ que se ha conectado.

Por ejemplo, para aceptar una conexión entrante y obtener un nuevo descriptor de archivo para la conexión:

```cpp
sockaddr_in source_address{};
socklen_t source_address_length;

// Poner el socket en modo de escucha
int result = listen(sock_fd, 5);
if (result < 0)
{
    // Error al poner el socket en modo de escucha...
}

// Aceptar la primera conexión que llegue
int new_fd = accept(sock_fd, reinterpret_cast<sockaddr*>(&source_address),
    &source_address_length);
if (new_fd < 0)
{
    // Error al aceptar la conexión...
}
```

Se puede llamar a `accept()` varias veces para aceptar varias conexiones entrantes y obtener un nuevo descriptor de archivo para cada una de ellas.
Por eso es un patrón muy común utilizar `accept()` en un bucle para aceptar conexiones entrantes en un _socket_ en modo de escucha:

```cpp
int result = listen(sock_fd, 5);
if (result < 0)
{
    // Error al poner el socket en modo de escucha...
}

while (true)
{
    sockaddr_in source_address{};
    socklen_t source_address_length;

    int new_fd = accept(sock_fd, reinterpret_cast<sockaddr*>(&source_address),
        &source_address_length);
    if (new_fd < 0)
    {
        // Error al aceptar la conexión...
    }

    // Procesar la nueva conexión...
}
```

### Envío y recepción de mensajes

El _socket_ devuelto por `accept()` se puede usar para enviar y recibir mensajes con el _socket_ al que se ha conectado, utilizando las funciones `sendto()` y `recvfrom` que se han visto anteriormente.
En el caso de `sendto()` se ignora la dirección del destinatario.

Sin embargo, por lo general, es preferible usar [`send()`](https://manpages.debian.org/stretch/manpages-es/send.2.es.html) y [`recv()`](https://manpages.debian.org/stretch/manpages-es/recv.2.es.html) en lugar de `sendto()` y `recvfrom()` cuando se trabaja con _sockets_ `SOCK_STREAM`.

```c
int send(int sock_fd, const void* buffer, size_t length, int flags);
int recv(int sock_fd, void* buffer, size_t length, int flags);
```

Como se puede observar en las declaraciones de estas funciones, no se necesita indicar la dirección del destinatario o recibir la del emisor, ya que el _socket_ está conectado a otro _socket_ y solo puede enviar y recibir mensajes con este.

Por lo tanto, así es como se enviaría un mensaje a un _socket_ `SOCK_STREAM` conectado:

```cpp
std::string message_text("¡Hola, mundo!");

int bytes_sent = send(new_fd, message_text.data(), message_text.size(), 0);
if (bytes_sent < 0)
{
    // Error al enviar el mensaje...
}
```

Y así es como se recibiría un mensaje de un _socket_ `SOCK_STREAM` conectado:

```cpp
std::string message_text();
message_text.resize(100);

int bytes_read = recv(new_fd, message_text.data(), message_text.size(), 0);
if (bytes_read < 0)
{
    // Error al recibir el mensaje...
}

message_text.resize(bytes_read);
std::println("Otro sistema envió el mensaje '{}'", message_text);
```