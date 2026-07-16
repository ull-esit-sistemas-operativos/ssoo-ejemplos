# Comunicación mediante paso de mensajes

Ejemplos de distintos mecanismos de comunicación entre procesos mediante paso de mensajes, cada uno en su propio subdirectorio:

- [`mqueue/`](mqueue/) — Colas de mensajes POSIX: incluye una clase en C++ que envuelve la API de colas de mensajes del sistema, y un servidor y un cliente que se comunican a través de ella.
- [`signals/`](signals/) — Señales POSIX: un programa que instala manejadores para varias señales y provoca una violación de segmento para verlos en acción.
- [`sockets/`](sockets/) — _Sockets_ de dominio UNIX: un servidor y un programa de control que se comunican a través de ellos.
- [`tuberías/`](tuberías/) — Tuberías anónimas y con nombre: comunicación entre un proceso padre y su hijo, redirección de la E/S estándar, y un servidor controlado remotamente mediante una tubería con nombre (FIFO).

Cada subdirectorio tiene su propio `README.md` con más detalles sobre los ejemplos que contienen.
