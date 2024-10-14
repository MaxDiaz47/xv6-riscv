#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void itoa(int n, char *str) {
  int i, sign;
  if ((sign = n) < 0)  // grab the sign
    n = -n;            // make n positive
  i = 0;
  do {  // generate digits in reverse order
    str[i++] = n % 10 + '0';
  } while ((n /= 10) > 0);
  if (sign < 0)
    str[i++] = '-';
  str[i] = '\0';
}

int main() {
  int pid;
  char buffer[50];

  // Crear 20 procesos hijos
  for (int i = 0; i < 20; i++) {
    pid = fork();  // Crear un proceso hijo
    
    if (pid == 0) {  // Si es el proceso hijo
      sleep(i);  // Pausar el proceso hijo según su número (i) para escalonar las impresiones
      itoa(getpid(), buffer);  // Convertir el PID a cadena
      write(1, "Ejecutando proceso con PID: ", 28);  // Mensaje de texto
      write(1, buffer, strlen(buffer));  // Escribir el PID
      write(1, "\n", 1);  // Nueva línea
      sleep(1);  // Pausar por 1 segundo antes de finalizar
      exit(0);   // Finaliza el proceso hijo
    }
  }
  
  // Esperar a que todos los procesos hijos terminen
  for (int i = 0; i < 20; i++) {
    wait(0);  // Esperar que cada proceso hijo termine
  }

  exit(0);  // Finalizar el proceso padre
}
