# Archivos mapeados en memoria

El mapeo de archivos en memoria es una técnica de E/S eficiente que permite a los programas acceder y modifica los datos en un archivo como estuvieran en la memoria, sin tener que leer o escribir explícitamente en el archivo usando funciones como `read()` o `write()`.
Esto se realiza con las llamadas al sistema `mmap()` y `munmap()`.

**Tabla de contenidos**
- [Mapear archivos](#mapear-archivos)
  - [Uso de `mmap()`](#uso-de-mmap)
- [Desmapear archivos](#desmapear-archivos)
  - [Uso de `munmap()`](#uso-de-munmap)
- [Ejemplo](#ejemplo)

## Mapear archivos

La función [`mmap()`](https://manpages.debian.org/stretch/manpages-es/mmap.2.es.html) mapea el archivo indicado en el descriptor de archivo `fd` —el quinto argumento de la función— en el espacio de direcciones del proceso:

```c
void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
```

Los argumentos de la función `mmap()` son los siguientes:

- `addr`: Dirección de memoria donde se desea mapeará el archivo.
  Si es `NULL`, el sistema operativo elegirá la dirección por nosostros.
- `length`: Tamaño de la región del archivo que se mapeará.
  Si se quiere mapear todo el archivo, se debe indicar el tamaño de este.
- `prot`: Permisos de acceso a la memoria en la región mapeada.
  Algunos de los valores posibles son `PROT_READ`, `PROT_WRITE` y `PROT_EXEC`.
- `flags`: Opciones adicionales para el mapeo.
  Algunos valores posibles son:
  - `MAP_SHARED` mapea el archivo de forma compartida.
    Es decir, los cambios en la memoria se reflejarán en el archivo y serán visibles para otros procesos que hayan mapeado el mismo archivo.
  - `MAP_PRIVATE` mapea el archivo de forma privada.
    Es decir, los cambios en la memoria no se reflejarán en el archivo y no serán visibles para otros procesos.
- `fd`: Descriptor de archivo del archivo que se mapeará.
- `offset`: Desplazamiento en el archivo donde se iniciará el mapeo.
  Si se quiere mapear todo el archivo, se debe indicar 0, para empezar desde el principio.

### Uso de `mmap()`

Este es un ejemplo muy sencillo de cómo mapear un archivo en memoria:

```cpp
// Primero se abre el archivo, con los permisos adecuados según si solo se leerá o también se escribirá.
int fd = open("archivo.txt", O_RDONLY);
if (fd < 0)
{
    // Error al abrir el archivo...
}

// Para mapear una archivo completo es necesario conocer su tamaño.
// Una forma es usar fstat() y otra es usar lseek().
// La función lseek() sirve para mover el puntero de lectura/escritura de un archivo y retorna la posición
// a la que se ha movido. Por tanto, si se mueve al final del archivo, se obtiene el tamaño de este.
int length = lseek( fd, 0, SEEK_END );

// Se mapea el archivo completo en memoria para solo lectura y de forma privada.
void* mem = mmap(NULL, length, PROT_READ, MAP_PRIVATE, fd, 0);
if (mem == MAP_FAILED)
{
    // Error al mapear el archivo...
}

// Opcionalmente, se puede cerrar el descriptor del archivo si ya no se necesita.
close(fd);

// Ahora se puede acceder a los datos del archivo como si estuvieran en la memoria.
// Por ejemplo, imprimir los primeros 10 caracteres por la consola.
std::string_view first_ten_chars(static_cast<char*>(mem), 10);
std::println(first_ten_chars);
```

## Desmapear archivos

Una vez se ha terminado de trabajar con el mapeo en memoria, se debe liberar con [`munmap()`](https://manpages.debian.org/stretch/manpages-es/munmap.2.es.html):

```c
int munmap(void *addr, size_t length);
```

Los argumentos de la función `munmap()` son los siguientes:

- `addr`: Dirección de memoria de la región mapeada que se quiere desmapear.
- `length`: Tamaño de la región mapeada que se liberará.

No es necesario desmapear la región completa mapeada.
Es decir, se puede desmapear solo una parte de esta.

### Uso de `munmap()`

Por ejemplo, para liberar el mapeo completo del archivo anterior, se debe llamar a `munmap()` con la dirección de memoria devuelta por `mmap` y el tamaño del archivo:

```cpp
if (munmap(mem, length) < 0)
{
    // Error al desmapear el archivo...
}
```

Y tampoco se debe olvidar cerrar el descriptor del archivo cuando se haya terminado, sino se ha cerrado antes:

```cpp
close(fd);
```

## Ejemplo

El archivo [mmapped-files.cpp](mmapped-files.cpp) contiene un ejemplo del uso de `mmap()` y `munmap()` para hacer la copia de un archivo.