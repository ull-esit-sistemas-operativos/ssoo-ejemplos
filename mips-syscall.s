# mips-syscall.s - Ejemplo de llamada al sistema en SPIM

      .data
str:  .asciiz "the answer = "
      .text
main: li $v0, 4        # system call code for print_str
      la $a0, str      # address of string to print
      syscall          # print the string
      
      li $v0, 10
      syscall