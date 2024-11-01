# Sistemas de archivos

**Tabla de contenidos**
- [Operaciones con archivos](#operaciones-con-archivos)
  - [Abrir archivos](#abrir-archivos)
    - [Opciones de apertura](#opciones-de-apertura)
    - [Permisos de los nuevos archivos](#permisos-de-los-nuevos-archivos)
  - [Cerrar descriptores de archivo](#cerrar-descriptores-de-archivo)
  - [Leer y escribir datos](#leer-y-escribir-datos)
    - [Ejemplo de `open()`, `read()` y `write()`](#ejemplo-de-open-read-y-write)
  - [Acceder a los atributos de un archivo](#acceder-a-los-atributos-de-un-archivo)
    - [Ejemplo de `stat()`](#ejemplo-de-stat)
    - [Comprobar si dos archivos son el mismo](#comprobar-si-dos-archivos-son-el-mismo)
    - [Acceso a los permisos y al tipo de archivo](#acceso-a-los-permisos-y-al-tipo-de-archivo)
- [Operaciones con directorios](#operaciones-con-directorios)
- [Bloqueos de archivo](#bloqueos-de-archivo)

## Operaciones con archivos

### Abrir archivos

Muchas de las funciones de la librería del sistema para manipular archivo requieren un descriptor de archivo.
Este descriptor es un número entero que identifica un archivo abierto en el sistema de archivos y que se puede obtener fácilmente mediante llamando a la función [`open()`](https://manpages.debian.org/stretch/manpages-es/open.2.es.html).

```c
int open(const char *pathname, int flags);
int open(const char *pathname, int flags, mode_t mode);
``` 

La función [`open()`](https://manpages.debian.org/stretch/manpages-es/open.2.es.html) abre el archivo cuya ruta se pasa como primer argumento (`pathname`) y devuelve, en caso de éxito, un descriptor de archivo.

#### Opciones de apertura

El segundo argumento (`flags`) es un conjunto de opciones que indican cómo se va a abrir el archivo.
Algunas de estas opciones son:

* `O_RDONLY` abre el archivo para lectura.
* `O_WRONLY` abre el archivo para escritura.
* `O_RDWR`: abre el archivo para lectura y escritura.

* `O_TRUNC` trunca el archivo a 0 bytes al abrirlo.
* `O_APPEND` abre el archivo para escritura y coloca el puntero de escritura al final del archivo.
Esto es útil cuando se quieren añadir datos al final del archivo.

* `O_CREAT` crea el archivo si no existe.
Si no se indica esta opción, y el archivo no existe, la función `open()` fallará con `ENOENT`.

* `O_EXCL` en combinación con `O_CREAT`, si el archivo ya existe, la llamada a `open()` falla con `EEXIST`

Se pueden combinar varias de estas opciones usando el operador `'|'` --_or_ bit a bit--:

```cpp
int fd = open(filename, O_RDONLY | O_CREAT);
if (fd == -1)
{
    // Error al abrir el archivo...
}
```

#### Permisos de los nuevos archivos

El tercer argumento (`mode`) indica los permisos del nuevo archivo, si es que va a ser creado porque no existía previamente.
Por tanto, esta opción solo tiene sentido si se pasa la opción `O_CREAT` en `flags` porque, de lo contrario, el archivo abierto ya existe y conserva sus permisos originales.

Si se crea un archivo nuevo y no se indica `mode`, el archivo se creará con permisos `0000`, es decir, sin permisos para nadie.
Así que lo adecuada es indicar siempre los permisos que se quiere que tenga el archivo, para lo que se puede usar la misma notación octal que se utiliza con el comando `chmod` en la _shell_:

```cpp
int fd = open(filename, O_RDONLY | O_CREAT, 0666);
if (fd == -1)
{
    // Error al abrir el archivo...
}
```

En el ejemplo anterior, el nuevo archivo hereda del proceso que llama a `open()` el propietario y el grupo.
Respecto a los permisos, el deseo del programador es que el archivo tenga permisos de lectura y escritura para el propietario, y permisos de lectura para el grupo y para otros.

Sin embargo, los permisos indicados en `mode` son modificados por la máscara **umask** del proceso antes ser aplicados al archivo, de tal forma que los permisos reales del archivo serán `mode & ~umask`.

La **umask** puede ser configurada por cada usuario mediante el comando `umask` de la shell:

```bash
$ umask 0022
```

Que también se puede usar para comprobar el valor actual de la máscara:

```bash
$ umask
0022
```

Todos los procesos lanzados por un usuario heredan la **umask** configurada en la shell, lo que permite que los usuarios controlen los permisos que tendrán los archivos que creen.
Por ejemplo, si un programa crea un nuevo archivo con `mode = 0666` pero la `umask` heredada por el proceso desde la _shell_ del usuario es `0022`, el archivo tendrá permisos `0666 & ~0022 = 0644`.
Es decir, con esa máscara, por defecto, los nuevos archivos no tendrán permisos de escritura ni para el grupo y ni para otros.

> [!NOTE]
> No tiene sentido indicar permisos de ejecución en `mode` --por ejemplo, `0777`-- a menos que se esté desarrollando un compilador y se sepa que el archivo que se va a crear va a ser ejecutable.

### Cerrar descriptores de archivo

Los descriptores de archivo se cierran usando la función [`close()`](https://manpages.debian.org/stretch/manpages-es/close.2.es.html).

El motivo de error más común al llamar a `close()` es intentar cerrar un descriptor inválido o que ya ha sido cerrado.

### Leer y escribir datos

Las funciones [`read()`](https://manpages.debian.org/stretch/manpages-es/read.2.es.html) y [`write()`](https://manpages.debian.org/stretch/manpages-es/write.2.es.html) permiten leer y escribir datos de un archivo abierto.
Solo hacer falta indicarle el descriptor del archivo abierto, la direccion de memoria donde se almacenarán los datos y el tamaño máximo de los datos a leer o escribir en esa región.

El valor de retorno de estas funciones, indica el número de bytes leídos o escritos por `read()` o `write()`, respectivamente, si no se ha producido ningún error.

Hay varios motivos por los que la función `write()` puede escribir menos bytes de los que se le han indicado, cuando se usa para escribir en un _socket_ o en una tubería.
Sin embargo, en el caso de un archivo, lo más común es que se deba a que no queda espacio suficiente en el dispositivo.

Lo correcto es comprobar el valor de retorno de `write()` y si es menor que el tamaño del buffer indicado, volver a llamar a `write()` con el resto de bytes que quedan por escribir.
Si el problema persiste, entonces sí retornará un -1 y el valor de `errno` indicará el error --por ejemplo, `ENOSPC` si el espacio en el dispositivo no es suficiente--.

Por otro lado, la función `read()` retorna el número de bytes realmente leídos, que puede ser inferior al número de bytes solicitados.
Por tanto, es frecuente tener que llamar a `read()` en un bucle hasta, que se hayan leído todos los datos que se necesitan.
En ese caso, cuando `read()` retorne 0 --por lo que el `buffer` devuelto por `read_file()` estará vacío-- se habrá alcanzado el final del archivo, por lo que ya no hay más datos que leer.

#### Ejemplo de `open()`, `read()` y `write()`

En el archivo [file-copy.cpp](file-copy.cpp) se muestra un ejemplo de cómo copiar un archivo en otro usando las funciones `open()`, `read()` y `write()`.

### Acceder a los atributos de un archivo

Las funciones [`stat()`](https://manpages.debian.org/stretch/manpages-es/stat.2.es.html) y `fstat()` ofrecen una manera de comprobar la existencia de un archivo y de obtener acceso a sus atributos para obtener información como:

* Tipo de archivo.
* Tamaño y número de bloques ocupados.
* Número de inodo.
* Propietario y grupo.
* Permisos.
* Fechas de acceso y modificación.

```c
int stat(const char* file_name, struct stat* buf);
int fstat(int filedes, struct stat* buf);
```

Si `stat()` o `fstat()` fallan con el error `ENOENT`, es que el archivo por el que se pregunta no existe.

Ambas versiones de `stat()` reciben un puntero a una estructura `stat`, que se rellena con la información del archivo al volver de la llamada.
La única diferencia entre `stat()` y `fstat()` es que `stat()` recibe la ruta del archivo y `fstat()` recibe el descriptor de archivo que se debe haber abierto previamente con `open()`.

La estructura `stat` contiene información sobre el archivo, como el número de inodo, los permisos, el tamaño, el número de enlaces, el propietario, el grupo o las fechas de acceso y modificación:

```{.cpp}
struct stat {
    dev_t         st_dev;      /* dispositivo */
    ino_t         st_ino;      /* inodo */
    mode_t        st_mode;     /* protección y tipo */
    nlink_t       st_nlink;    /* número de enlaces físicos */
    uid_t         st_uid;      /* ID del usuario propietario */
    gid_t         st_gid;      /* ID del grupo propietario */
    dev_t         st_rdev;     /* tipo dispositivo (si es
                                  dispositivo inodo) */
    off_t         st_size;     /* tamaño total, en bytes */
    blksize_t     st_blksize;  /* tamaño de bloque para el
                                  sistema de ficheros de E/S */
    blkcnt_t      st_blocks;   /* número de bloques asignados */
    time_t        st_atime;    /* hora último acceso */
    time_t        st_mtime;    /* hora última modificación */
    time_t        st_ctime;    /* hora último cambio */
};
```

#### Ejemplo de `stat()`

En el archivo [file-attribs.cpp](file-attribs.cpp) se muestra un ejemplo de cómo obtener y mostrar información de un archivo usando la función `stat()`.

#### Comprobar si dos archivos son el mismo

Cada archivo tiene un número de inodo único que lo identifica en el sistema de archivos.
Un mismo archivo puede ser accesible por medio de varias rutas, por lo que se pueden comparar los números de inodos de archivos en rutas diferentes para determinar si son el mismo archivo.

El inconveniente es que el número de inodo solo es único dentro del mismo sistema de archivos, es decir, que dos archivos pueden tener el mismo número de inodo en sistemas de archivos diferentes.
Por eso se necesita comparar tanto `st_dev` --que identifica el dispositivo donde se encuentra el archivo-- como `st_ino` --que identifica el archivo dentro del dispositivo-- de la estructura `stat`, para saber si dos rutas conducen al mismo archivo.

#### Acceso a los permisos y al tipo de archivo

Tanto los permisos como el tipo del archivo se almacenan en el campo `st_mode` de la estructura `stat`.
Para comprobar si un archivo es de un tipo en particular, se puede utilizar alguna de las siguientes macros:

* `S_ISLNK(m)`: ¿Es un enlace simbólico?
* `S_ISREG(m)`: ¿En un fichero regular?
* `S_ISDIR(m)`: ¿Es un directorio?
* `S_ISCHR(m)`: ¿Es un dispositivo de caracteres?
* `S_ISBLK(m)`: ¿Es un dispositivo de bloques?
* `S_ISFIFO(m)`: ¿Es una tubería con nombre (_FIFO_)?
* `S_ISSOCK(m)`: ¿Es un _socket_ de dominio UNIX con nombre?

Por ejemplo:

```cpp
stat st;
if (stat(filepath, &st) == -1)
{
    // Error al obtener los atributos del archivo...
}
else
{
    if (S_ISLNK(st.st_mode))
    {
        // 'filepath' es un enlace simbólico
    }
    else {
        // 'filepath' no es un enlace simbólico
    }
}
```

La macro `S_IFMT` contiene una máscara con todos bits que sirven para indicar el tipo de archivo a 1.
Es decir, que se puede usar `S_IFMT` para extraer por separado los bits de tipo y los permisos del archivo:

```cpp
mode_t file_type = st.st_mode & S_IFMT;
mode_t file_permissions = st.st_mode & ~S_IFMT;
```

## Operaciones con directorios

El archivo [dir-list.cpp](dir-list.cpp) contiene un programa que lista todos los archivos de un directorio usando las funciones `opendir()`, `readdir()` y `closedir()`.

## Bloqueos de archivo

El archivo [filelock-server.c](filelock-server.c) contiene un ejemplo de cómo se pueden utilizar los bloqueos de archivo para sincronizar el acceso a un archivo compartido entre varios procesos.

El programa utiliza `alarm()` y las señales del sistema para mostrar periódicamente la hora.
Además, crea un archivo con el PID del proceso.
Este archivo es bloqueado durante su creación para que solo un proceso pueda escribir su PID en él.
Esto permite a otros procesos del mismo programa detectar si el archivo ya existe y terminar inmediatamente, asegurando que solo hay un proceso de ese mismo programa ejecutándose a la vez.

El programa de control [filelock-stop.cpp](filelock-stop.cpp) puede usar este archivo para conocer el PID para enviar una señal al servidor y hacer que termine.

Esta técnica es muy usada por los servicios del sistema.
Frecuentemente, crean un subdirectorio con el nombre del servicio dentro del directorio `/var/run` y allí colocan un archivo `.pid` con el PID del proceso; así como otros recursos necesarios para la comunicación con el servicio, como *sockets* de dominio UNIX o FIFO.
Este archivo `.pid` permite a los clientes saber si el servicio está en ejecución y mandarle señales para detenerlo o reiniciarlo.
