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

En el archivo [`fork-pipe.cpp`](posix/fork-pipe.cpp) se muestra un ejemplo de cómo crear una tubería sin nombre y comunicar dos procesos mediante ella.

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

En los archivos [`fifo.cpp`](posix/fifo.cpp) y [`fifo-control.cpp`](posix/fifo-control.cpp) se muestra un ejemplo de cómo crear una tubería con nombre y comunicar dos procesos mediante ella.

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

## Lectura y escritura en tuberías

Para leer de una tubería se utiliza la función [`read()`](https://manpages.debian.org/stretch/manpages-es/read.2.es.html) y para escribir se utiliza la función [`write()`](https://manpages.debian.org/stretch/manpages-es/write.2.es.html).

La lectura de una tubería en la que no hay datos disponibles bloqueará el proceso hasta que haya datos para leer.
Cuando se cierran todos los descriptores de archivo de escritura de una tubería y se han leído todos los datos, la llamada a `read()` devuelve 0, indicando que no hay más datos que leer.

## Redirección de la entrada y salida estándar

La función `dup2()` y la redirección de la E/S estándar de un proceso a un archivo se explican en el [capítulo 9](../../cap09/README.md#redirección-de-la-es-estándar), junto con el ejemplo [`fork-dup2.cpp`](../../cap09/posix/fork-dup2.cpp).
Aquí se aplica el mismo mecanismo a las tuberías.

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

En el archivo [`fork-redir.cpp`](posix/fork-redir.cpp) se muestra un ejemplo de cómo redirigir la salida estándar de un proceso hijo a una tubería y leerla en el proceso padre.

## En Windows

La API de Windows también tiene tuberías anónimas, que se crean con [`CreatePipe()`](https://learn.microsoft.com/en-us/windows/win32/api/namedpipeapi/nf-namedpipeapi-createpipe) y se leen y escriben con `ReadFile()` y `WriteFile()`.
Lo que cambia realmente en estos ejemplos no es la tubería, sino la manera de crear el proceso con el que se comparte.

### No hay fork()

En Windows no se puede duplicar el proceso actual: [`CreateProcess()`](https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-createprocessa) hace de una vez lo que en POSIX hacen `fork()` y `exec()` juntos, y siempre arranca un programa desde cero.
Eso tiene tres consecuencias que se ven muy bien en los ejemplos:

1. **El hijo no hereda la memoria del padre.** En [`fork-pipe.cpp`](posix/fork-pipe.cpp) el hijo ya tiene dentro el número que escribió el usuario y el descriptor de la tubería, porque es una copia del padre.
   En la versión de Windows hay que pasárselos por la línea de comandos.
2. **No hay un código «entre medias» donde preparar las cosas.** En POSIX, el código que va del `fork()` al `exec()` se ejecuta ya dentro del hijo, y es ahí donde [`fork-redir.cpp`](posix/fork-redir.cpp) redirige la salida estándar con `dup2()`.
   En Windows todo hay que pedirlo por adelantado, al crear el proceso.
3. **El hijo puede terminar con `return`.** La regla de terminar el hijo con `_exit()` es para no vaciar por duplicado los búferes heredados del padre.
   Un proceso creado con `CreateProcess()` no hereda ninguno, así que el problema no existe.

Para que el hijo ejecute el mismo código que el padre, como hace `fork()`, en [windows/createprocess-pipe.cpp](windows/createprocess-pipe.cpp) el programa se lanza a sí mismo con un argumento de línea de comandos que le dice a la copia que le toca hacer de hijo.

### La herencia se pide, no se hereda

En POSIX el hijo hereda todos los descriptores del padre salvo que se diga lo contrario, y por eso los ejemplos cierran con `close()` los extremos de la tubería que no necesitan.
En Windows es al revés: no se hereda nada salvo que se pida, y se pide en tres sitios a la vez.

1. Al crear la tubería, pasando una `SECURITY_ATTRIBUTES` con `bInheritHandle = TRUE`.
2. Quitando la marca de heredable, con [`SetHandleInformation()`](https://learn.microsoft.com/en-us/windows/win32/api/handleapi/nf-handleapi-sethandleinformation), al extremo que el hijo no debe heredar.
3. Poniendo a `TRUE` el parámetro `bInheritHandles` de `CreateProcess()`, sin el cual no se hereda ningún manejador por mucho que se hayan marcado.

### Correspondencia entre las funciones

| POSIX | Windows |
| --- | --- |
| `pipe()` | [`CreatePipe()`](https://learn.microsoft.com/en-us/windows/win32/api/namedpipeapi/nf-namedpipeapi-createpipe) |
| `fork()` + `exec()` | [`CreateProcess()`](https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-createprocessa) |
| `close()` de lo que no se hereda | [`SetHandleInformation()`](https://learn.microsoft.com/en-us/windows/win32/api/handleapi/nf-handleapi-sethandleinformation) antes de crear el proceso |
| `dup2()` sobre `STDOUT_FILENO` | `STARTF_USESTDHANDLES` y `hStdOutput` en `STARTUPINFO` |
| `wait()` | [`WaitForSingleObject()`](https://learn.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-waitforsingleobject) |
| `WEXITSTATUS()` | [`GetExitCodeProcess()`](https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-getexitcodeprocess) |
| Fin de archivo: `read()` devuelve 0 | `ReadFile()` falla con `ERROR_BROKEN_PIPE` |

Los ejemplos están en [windows/createprocess-pipe.cpp](windows/createprocess-pipe.cpp) y [windows/createprocess-redir.cpp](windows/createprocess-redir.cpp).

### Tuberías con nombre

La API de Windows llama a las tuberías con nombre *named pipes* y las crea con [`CreateNamedPipe()`](https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-createnamedpipea).
Al cliente se le parecen mucho a un archivo —las abre con `CreateFile()`, como cualquier otro—, pero al servidor no tanto.

| POSIX | Windows |
| --- | --- |
| `mkfifo()` + `open()` | [`CreateNamedPipe()`](https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-createnamedpipea) |
| — | [`ConnectNamedPipe()`](https://learn.microsoft.com/en-us/windows/win32/api/namedpipeapi/nf-namedpipeapi-connectnamedpipe) |
| `open()` en el cliente | [`CreateFile()`](https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilea) |
| `close()` | [`DisconnectNamedPipe()`](https://learn.microsoft.com/en-us/windows/win32/api/namedpipeapi/nf-namedpipeapi-disconnectnamedpipe) y `CloseHandle()` |
| `unlink()` | — |

Las diferencias de fondo son tres.

**No están en el sistema de archivos.** Una tubería FIFO de POSIX es un archivo más, con su ruta y sus permisos, que hay que borrar con `unlink()` cuando ya no hace falta.
Las de Windows viven en un espacio de nombres propio, al que se llega con rutas de la forma `\.\pipe\<nombre>`, y desaparecen solas al cerrarse su último manejador.
Ese `.` indica el equipo local, y ahí puede ir el nombre de otro equipo, pues estas tuberías funcionan también a través de la red.

**Están orientadas a la conexión.** En POSIX la tubería no distingue clientes: cualquiera que abra el archivo escribe en ella y lo que llega se mezcla.
En Windows el servidor espera con `ConnectNamedPipe()` a que llegue un cliente, lo atiende, y lo despide con `DisconnectNamedPipe()` para poder atender al siguiente.
Por eso el bucle del ejemplo tiene dos niveles: uno para los clientes y otro para los comandos de cada cliente.

**Hay que decir de antemano cómo se va a usar la tubería.** `CreateNamedPipe()` recibe en qué sentido van los datos, si se tratan como una secuencia de bytes o como mensajes sueltos, y cuántos clientes puede haber a la vez.
En POSIX la FIFO es siempre un flujo de bytes y esas decisiones no existen.

Los ejemplos están en [windows/namedpipe.cpp](windows/namedpipe.cpp) y [windows/namedpipe-control.cpp](windows/namedpipe-control.cpp).
