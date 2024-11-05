# Tuberías

Las tuberías son un mecanismo de comunicación entre procesos.

Conceptualmente, cada tubería tiene dos extremos en los que opera utilizando la misma interfaz que generalmente empleamos para manipular archivos.
Es decir, se envían datos a través de la tubería mediante una llamada a `write()` en uno de los extremos y se reciben datos en el otro mediante una llamada a `read()`.

El que cada extremo se comporte como un archivo, facilita que se puedan usar en muchas de las llamadas al sistema que aceptan un archivo como argumento. Los procesos pueden leer o escribir en un archivo sin saber realmente si están accediendo a un archivo real o se están comunicando con otro proceso mediante una tubería.

Existen dos tipos de tuberías:

- **Tuberías sin nombre** o **anónimas**: Son las más comunes y se crean mediante la llamada al sistema [`pipe()`](https://manpages.debian.org/stretch/manpages-es/pipe.2.es.html).
  Se utilizan para la comunicación entre procesos relacionados, como un proceso padre y sus hijos.

- **Tuberías con nombre**: Se crean mediante la llamada al sistema [`mkfifo()`](https://manpages.debian.org/stretch/manpages-es/mkfifo.2.es.html).
  Se utilizan para la comunicación entre procesos no relacionados, como dos procesos que se ejecutan de forma independiente.
  Por ejemplo, un proceso servidor que acepta conexiones de múltiples clientes.

## Creación de tuberías sin nombre

Para crear una tubería sin nombre, se utiliza la llamada al sistema [`pipe()`](https://manpages.debian.org/stretch/manpages-es/pipe.2.es.html).

```c
int pipe(int pipefd[2]);
```

La función `pipe()` crea una tubería sin nombre y devuelve dos descriptores de archivo en el array `pipefd`:

- `pipefd[0]`: Descriptor de archivo para lectura.
- `pipefd[1]`: Descriptor de archivo para escritura.

Por ejemplo, el siguiente código crea una tubería sin nombre y obtiene los descriptores de archivo para lectura y escritura:

```cpp
int pipefd[2];

int result = pipe(pipefd);
if (result < 0)
{
    // Error al crear la tubería...
}

std::println("Descriptor de archivo para lectura: {}", pipefd[0]);
std::println("Descriptor de archivo para escritura: {}", pipefd[1]);
```

Como los descriptores de archivo abiertos se heredan de procesos padre a procesos hijo, estas tuberías se pueden utilizar para la comunicación entre procesos ellos.
Al hacer `fork()` tras llamar a `pipe()` ambos procesos tendrán acceso a los descriptores de archivo de la tubería, por lo que pueden comunicarse entre ellos.

En el archivo [`fork-pipe.cpp`](fork-pipe.cpp) se muestra un ejemplo de cómo crear una tubería sin nombre y comunicar dos procesos mediante ella.

## Creación de tuberías con nombre

Para crear una tubería con nombre, se utiliza la función [`mkfifo()`](https://manpages.debian.org/stretch/manpages-es/mkfifo.2.es.html), a la que se le pasa como argumento la ruta del archivo tipo FIFO que representa la tubería y los permisos de acceso.

```cpp
int mkfifo(const char *pathname, mode_t mode);
```

Después de crear la tubería, se pueden abrir los descriptores de archivo para lectura y escritura con la función [`open()`](https://manpages.debian.org/stretch/manpages-es/open.2.es.html).

Por ejemplo, para crear la tubería `/tmp/myfifo` y abrir el descriptor de archivo para escritura:

```cpp
// Crear la tubería con nombre
mkfifo("/tmp/myfifo", 0666);

// Abrir el descriptor de archivo para escritura
int fd = open(fifo_path, O_WRONLY);
if (fd < 0)
{
    // Error al abrir el descriptor de archivo...
}

// Escribir datos en la tubería
// ...

// Cerrar el descriptor de archivo cuando ya no sea necesario
close(fd);

// Eliminar la tubería con nombre
unlink("/tmp/myfifo");
```

En los archivos [`fifo.cpp`](fifo.cpp) y [`fifo-control.cpp`](fifo-control.cpp) se muestra un ejemplo de cómo crear una tubería con nombre y comunicar dos procesos mediante ella.

## Cerrar descriptores de archivo de tuberías

Los descriptores de archivo de las tuberías deben cerrarse cuando ya no se necesiten para liberar los recursos asociados a ellos.

Esto se hace llamando a la función [`close()`](https://manpages.debian.org/stretch/manpages-es/close.2.es.html) con el descriptor de archivo como argumento.

```cpp
close(fd);
```

## Eliminar tuberías con nombre

Las tuberías con nombre se crean como archivos en el sistema de archivos y pueden eliminarse de este cuando ya no se necesiten.

La función [`unlink()`](https://manpages.debian.org/stretch/manpages-es/unlink.2.es.html) se utiliza para eliminar un archivo del sistema de archivos, incluidas las tuberías con nombre.

```cpp
unlink("/tmp/myfifo");
```

## Redirección de la entrada y salida estándar

### Duplicar descriptores de archivo

Los descriptores de archivo se pueden copiar, haciendo que varios descriptores de archivo apunten al mismo archivo o recursos.

Por ejemplo, si se abre un archivo y se copia el descriptor de archivo a 42, ahora el archivo tiene dos descriptores de archivo abiertos, uno en el valor de `fd` y otro en 42.

```cpp
int fd = open("archivo.txt", O_RDONLY);
if (fd < 0)
{
    // Error al abrir el archivo...
}

// Copiar el descriptor de archivo en el 42
int fd2 = dup2(fd, 42);

// Leer datos del archivo con el descriptor de archivo 42
// Hubiera sido lo mismo usar fd2, ya que es igual a 42 al volver de dup2()
std::array<char, 1024> buffer;
int bytes_read = read(45, buffer, sizeof(buffer));
if (bytes_read < 0)
{
    // Error al leer del archivo...
}

close(fd);
close(45);
```

Para hacerlo, como se puede observar en el ejemplo anterior, se utiliza la función [`dup2()`](https://manpages.debian.org/stretch/manpages-es/dup.2.es.html), que copia el descriptor de archivo `oldfd` al descriptor de archivo `newfd`.
Si `newfd` ya estaba abierto, se cierra antes de copiar el descriptor de archivo.
Y si `dup2` tiene éxito, devuelve `newfd`.

### Redirección de la E/S estándar a un archivo

Este mecanismo se puede utilizar para redirigir la entrada y salida estándar de un proceso a un archivo, de forma que la salida del proceso se escriba en un archivo en lugar de en la consola.
O la entrada del proceso se lea de un archivo en lugar de desde la consola.

Por ejemplo, para redirigir la salida estándar de un proceso a un archivo:

```cpp
int fd = open("salida.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);
if (fd < 0)
{
    // Error al abrir el archivo...
}

// Redirigir la salida estándar al archivo
dup2(fd, STDOUT_FILENO);

std::println("Este mensaje se escribirá en el archivo 'salida.txt'");

close(fd);
```

En el ejemplo anterior se redirige la salida estándar del proceso porque se duplica `fd` sobre el descriptor de archivos `STDOUT_FILENO` (1), que es el descriptor de archivo que representa la salida estándar del proceso.
Se puede hacer lo mismo para redirigir la entrada estándar del proceso a un archivo, duplicando el descriptor de archivo sobre `STDIN_FILENO` (0) o redirigir la salida de error estándar a un archivo duplicando el descriptor de archivo sobre `STDERR_FILENO` (2).

Además, es importante que el descriptor del archivo se abra en el modo adecuado para la operación que se va a realizar.
Es decir, si se va a redirigir la salida estándar o de error a un archivo, el archivo debe abrirse en modo escritura, para poder escribir en el archivo la salida del programa.
Si se va a redirigir la entrada estándar desde un archivo, el archivo debe abrirse en modo lectura, para poder leer del archivo la entrada del programa.

### Redirección de la E/S estándar a una tubería

Este mecanismo también se puede utilizar para redirigir la entrada y salida estándar de un proceso a una tubería.
De esta forma la salida de un proceso se escribe en una tubería en lugar de en la consola o la entrada del proceso se lee de una tubería en lugar de desde la consola.

Por ejemplo, para redirigir la salida estándar de un proceso a una tubería:

```cpp
std::array<int, 2> pipefd;
int result = pipe(pipefd.data());
if (result < 0)
{
    // Error al crear la tubería...
}

// Cerrar el descriptor de archivo para lectura
close(pipefd[0]);

// Redirigir la salida estándar a la entrada de la tubería.
// pipefd[1] es el descriptor de archivo para escritura de la tubería.
dup2(pipefd[1], STDOUT_FILENO);

std::println("Este mensaje se escribirá en la tubería");

close(pipefd[1]);
```

Mientras que para redirigir la entrada estándar de un proceso a una tubería:

```cpp
std::array<int, 2> pipefd;
int result = pipe(pipefd.data());
if (result < 0)
{
    // Error al crear la tubería...
}

// Cerrar el descriptor de archivo para escritura
close(pipefd[1]);

// Redirigir la entrada estándar a la salida de la tubería.
// pipefd[0] es el descriptor de archivo para lectura de la tubería.
dup2(pipefd[0], STDIN_FILENO);

// Leer de la entrada estándar que ahora está redirigida a la tubería
std::string input;
std::cin >> input;

close(pipefd[0]);
```

En el archivo [`fork-redir.cpp`](fork-redir.cpp) se muestra un ejemplo de cómo redirigir la salida estándar de un proceso hijo a una tubería y leerla en el proceso padre.