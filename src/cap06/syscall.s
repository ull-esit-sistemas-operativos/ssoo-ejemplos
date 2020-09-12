# syscall.s - Llamada al sistema en Linux para x86-64
#
#   En Linux para x86-64 los argumentos de las llamadas al sistema
#   se mapean en registros así:
#
#       Id. llamada al sistema: rax
#       1er argumento:          rdi
#       2º argumento:           rsi
#       3er argumento:          rdx
#       4º argumento:           r10
#       5º argumento:           r8
#       6º argumento:           r9
#       Valor de retorno:       rax
#                   
#       No se soportan más de 6 argumentos.
#
#       Ver '/usr/include/x86_64-linux-gnu/asm/unistd_64.h' para conocer
#       el identificador de cada llamada al sistema. 
#

            .intel_syntax noprefix

            .global _start

            .data
message:    .ascii "¡Hola mundo!\n"
            message_len = . - message

            .text
_start:     mov rax, 1                  # Id. de write()
            mov rdi, 1                  # Descriptor de la salida estándar
            mov rsi, offset message     # Dirección de la variable con el mensaje
            mov rdx, message_len        # Número de bytes del mensaje
            syscall                     # Hacer la llamada al sistema write() para
                                        #  mostrar 'message' por la salida estándar 
      
            mov rax, 60                 # Id. de exit()
            mov rdi, 0                  # Valor de salida del proceso
            syscall                     # Hacer la llamada al sistema: exit(0)
