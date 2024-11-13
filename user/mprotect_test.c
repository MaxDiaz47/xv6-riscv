#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void probar_proteccion(void)
{
    // Solicitar 2 páginas de memoria
    char *addr = sbrk(2 * 4096);
    if (addr == (char *)-1)
    {
        printf("Error: fallo en sbrk\n");
        exit(1);
    }

    // Asignar y mostrar un valor inicial
    addr[0] = 'A';
    printf("Valor inicial asignado: %c\n", addr[0]);

    // Activar protección en la primera página
    if (mprotect(addr, 1) < 0)
    {
        printf("Error: fallo en mprotect\n");
        exit(1);
    }
    printf("Protección activada en la primera página\n");

    // Intentar escribir en la página protegida (esto debería fallar)
    printf("Intentando modificar la página protegida...\n");
    addr[0] = 'B'; // Aquí debería ocurrir un error de protección

    // Este mensaje no debería aparecer, ya que el programa debería fallar antes
    printf("Error: La protección de la página no funcionó\n");
    exit(1);
}

int main(void)
{
    printf("Iniciando prueba de protección de páginas de memoria...\n");
    probar_proteccion();
    return 0;
}
