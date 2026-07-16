# Señales POSIX

Las [señales](https://manpages.debian.org/stretch/manpages-es/signal.7.es.html) son la forma más sencilla que tienen los sistemas POSIX para comunicar dos procesos.
Un proceso puede mandar una señal a otro con la llamada al sistema [`kill()`](https://manpages.debian.org/stretch/manpages-es/kill.2.es.html), indicando el identificador del proceso de destino y el número que identifica la señal.

Cada señal tiene un efecto por defecto —normalmente terminar el proceso que la recibe—, pero un proceso puede declarar su propio **manejador de señal** con [`sigaction()`](https://manpages.debian.org/stretch/manpages-es/sigaction.2.es.html) para tratarla de otra forma.

En [`signals.cpp`](signals.cpp) se puede ver un programa de ejemplo que muestra cómo manejar señales del sistema y que sirve para ver cómo funcionan:

- Instala un manejador para `SIGTERM`, `SIGINT` y `SIGHUP` que se limita a imprimir qué señal ha llegado.
  Se usa `SA_RESTART` porque el programa está esperando la pulsación de una tecla con `getc()` y no interesa que deje de esperar cuando llega una de estas señales.

- Instala otro manejador para `SIGSEGV` que imprime un mensaje y termina el programa de forma ordenada con `exit()`, en vez de dejar que ocurra la terminación abrupta por defecto.

- Tras pulsar una tecla, provoca deliberadamente una violación de segmento escribiendo a través de un puntero nulo, para que se pueda ver en acción el manejador de `SIGSEGV`.

Solo hay que ejecutarlo y, mientras espera la pulsación de una tecla, enviarle señales con el comando `kill` desde otra terminal para ver cómo las gestiona.
