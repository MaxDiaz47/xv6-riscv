#include "kernel/types.h"
#include "user/user.h"

int main(void) {
    int ppid = getppid(); // Llamada a la función que implementaste
    printf("El ID del proceso padre es: %d\n", ppid);

    // Probar getancestor() con diferentes valores
    for (int i = 0; i <= 3; i++) {
        int ancestor = getancestor(i);
        if (ancestor == -1)
            printf("No hay un ancestro %d\n", i);
        else
            printf("El ancestro %d tiene el ID: %d\n", i, ancestor);
    }

    exit(0);
}