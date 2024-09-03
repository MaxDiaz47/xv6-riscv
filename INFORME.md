# Informe de Implementación de la Llamada al Sistema `getancestor`

## Introducción

En este proyecto se implementó una nueva llamada al sistema en el sistema operativo xv6 para obtener el ID de los ancestros de un proceso en ejecución. La llamada al sistema se denominó `getancestor`.

## Modificaciones Realizadas

### 1. Definición de la Llamada al Sistema

Se definió la llamada al sistema `getancestor` en varios archivos clave del kernel de xv6:

- **syscall.h**: Se añadió la definición del número de syscall para `getancestor`y getppid().

`#define SYS_getppid 21`
`#define SYS_getancestor 22`

- **syscall.c**: Se añadió la referencia a la función `sys_getancestor` en la tabla de llamadas al sistema.

`extern uint64 sys_getancestor(void);`
`[SYS_getancestor] sys_getancestor,`

- **sysproc.c**: Se implementó la función `sys_getancestor` que maneja la lógica de la syscall y la función sys_getppid() que retorne myproc()->parent->pid..

uint64
sys_getppid(void)
{
  return myproc()->parent->pid;
}

uint64
sys_getancestor(void)
{
    int n = 0;  // Inicia  n
    argint(0, &n); // Obtiene el argumento de la llamada al sistema

    if (n < 0) {
        return -1;  // Retorna -1 si n no es valido
    }

    struct proc *p = myproc();  // Obtén el proceso actual

    // Este ciclo recorre hacia atrás por la cantidad de ancestros solicitados
    for (int i = 0; i < n; i++) {
        if (p->parent == 0) {  // Si no hay más ancestros, retorna -1
            return -1;
        }
        p = p->parent;  // Recorre al proceso padre
    }

    return p->pid;  // Retorna el PID del ancestro correspondiente
}

### 2. Modificación del Archivo `user.h`

Se añadió el prototipo de la nueva función de syscall en el archivo `user.h` para que pudiera ser utilizada en los programas de usuario.

`int getppid(void);`
`int getancestor(int);`

### 3. Creación del Programa de Usuario `yosoytupadre.c`

Se creó un programa en C llamado `yosoytupadre.c` que hace uso de la nueva llamada al sistema `getancestor`. El programa imprime el ID del proceso padre y los IDs de sus ancestros hasta el nivel especificado.

`#include "kernel/types.h"`
`#include "user/user.h"`

`int main(void) {`
`int ppid = getppid();`
`printf("El ID del proceso padre es: %d\n", ppid);`

`for (int i = 0; i < 3; i++) {`
`int ancestor = getancestor(i);`
`if (ancestor != -1) {`
`printf("El ancestro %d tiene el ID: %d\n", i, ancestor);`
`} else {`
`printf("No hay un ancestro %d\n", i);`
`}`
`}`
`exit(0);`
`}`

## Resultados

El programa de prueba `yosoytupadre.c` se ejecutó exitosamente en el entorno xv6 modificado, mostrando el ID del proceso padre y los IDs de los ancestros hasta el tercer nivel.

`init: starting sh`
`$ yosoytupadre`
`El ID del proceso padre es: 2`
`El ancestro 0 tiene el ID: 3`
`El ancestro 1 tiene el ID: 2`
`El ancestro 2 tiene el ID: 1`
`No hay un ancestro 3`
`$` 