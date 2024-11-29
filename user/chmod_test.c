#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int main() {
  int fd = open("testfile", O_CREATE | O_RDWR);
  if (fd < 0) {
    printf("Error: no se pudo crear el archivo\n");
    exit(1);
  }

  write(fd, "hola", 4);
  close(fd);

  chmod("testfile", 1); // Cambiar a solo lectura

  fd = open("testfile", O_WRONLY);
  if (fd < 0)
    printf("Permisos respetados: no se puede abrir en modo escritura\n");
  else
    printf("Error: archivo debería ser solo lectura\n");

  chmod("testfile", 5); // Marcar como inmutable
  if (chmod("testfile", 3) < 0)
    printf("Archivo inmutable: no se puede cambiar permisos\n");

  unlink("testfile");
  exit(0);
}