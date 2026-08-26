# Sincronización

Ejemplos de los mecanismos que permiten coordinar el acceso de varios hilos a los mismos datos, cada uno en su propio subdirectorio:

- [`mutex/`](mutex/) — _Mutex_: un contador y un cálculo del factorial que varios hilos actualizan al mismo tiempo, protegiendo la sección crítica con un _mutex_. Cada ejemplo está en dos versiones, una con POSIX Threads y otra con la librería estándar de C++.
- [`semáforos/`](semáforos/) — Semáforos: una clase que implementa un semáforo a partir de un _mutex_ y una variable de condición, y un programa que la usa para limitar el número de hilos que entran a la vez en una sección crítica.

Cada subdirectorio tiene su propio `README.md` con más detalles sobre los ejemplos que contienen.
