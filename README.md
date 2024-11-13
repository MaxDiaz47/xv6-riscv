# Proyecto XV6 - Tarea 3

Este documento describe los pasos realizados para completar la tarea 3 de sistemas operativos, concretamente, `xv6-riscv`, junto con los cambios realizados en el código fuente, los archivos creados, y cómo se ejecuta el proyecto.

## 1. Descripción del Proyecto

En esta tarea, se debía modificar xv6 para implementar un sistema de protección de memoria que permita marcar regiones de memoria como solo lectura. Tambien, el objetivo del trabajo fue implementar las llamadas al sistema `mprotect` y `munprotect` en el sistema operativo `xv6-riscv` y realizar los ajustes necesarios para asegurar su funcionamiento sin errores.


## 2. Archivos Modificados y Creaciones Nuevas

1. **Archivo Creado: `user/mprotect_test.c`**:
   - Este código realiza una prueba para verificar la protección de memoria en un sistema que soporte mprotect. Primero, solicita 2 páginas de memoria mediante sbrk. Luego, asigna un valor inicial ('A') en la primera dirección y lo muestra en pantalla. A continuación, utiliza mprotect para proteger la primera página, lo que debería impedir futuras escrituras en esa sección de memoria. Para confirmar la protección, el código intenta escribir en la página protegida; si mprotect funciona correctamente, este intento debería causar un fallo en el programa (un error de protección de memoria). Así, el programa verifica que la protección se active y evite cambios en las páginas de memoria protegidas.
   
   ### Código del archivo `mprotect_test.c`:

    ```c
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

    ```

2. **Modificación en `Makefile`**:
   - Para que el programa `mprotect_test.c` sea compilado y ejecutado como un programa de usuario en `xv6`, fue necesario agregarlo al `Makefile` de la siguiente manera:
     - Se añadió la línea `$U/_mprotect_test\` en la sección `UPROGS`:
     ```makefile
     UPROGS=\
     	$U/_cat\
     	$U/_echo\
     	...
     	$U/_mprotect_test\
     ```

3. **Modificaciones en `sysproc.c`**: 

    - Para implementar las llamadas al sistema `mprotect` y `munprotect` en `xv6-riscv`, se añadieron funciones que protegen y desprotegen las páginas de memoria de un proceso. A continuación, se explican los cambios realizados y las nuevas funciones creadas.

    ***Función `validate_addr_len`*** 

    - Esta función realiza validaciones para asegurarse de que la dirección de inicio y el rango de memoria a proteger sean válidos.

    ```c
    static int validate_addr_len(void *addr, int len, struct proc *p) {
        uint64 va = (uint64)addr;

        // Revisa que la dirección inicial esté correctamente alineada al tamaño de una página
        if (va % PGSIZE != 0)
            return -1;

        // Asegúrate de que la longitud sea mayor que cero
        if (len <= 0)
            return -1;

        // Calcula el rango de memoria que debe protegerse y verifica que no supere el límite permitido
        uint64 end_va = va + (len * PGSIZE);
        if (va >= MAXVA || end_va > MAXVA || end_va < va)
            return -1;

        // Recorre cada página dentro del rango y verifica que esté mapeada en la tabla de páginas del proceso
        for (uint64 a = va; a < end_va; a += PGSIZE) {
            pte_t *pte = walk(p->pagetable, a, 0);
            if (pte == 0 || (*pte & PTE_V) == 0)
                return -1;  // Falla si la página no está mapeada o no es válida
        }

        return 0;  // Si todo está correcto, devuelve 0
    }
     ```

    ***Función `sys_mprotect`*** 

    - Esta función desactiva el permiso de escritura en las páginas especificadas.

    ```c
    uint64 sys_mprotect(void) {
    uint64 addr;
    int len;

    if (argaddr(0, &addr) < 0 || argint(1, &len) < 0) // Usando uint64 en lugar de void*
        return -1;

    // Validar dirección y longitud
    if (validate_addr_len((void *)addr, len, myproc()) < 0)
        return -1;

    // Recorre las páginas y modifica los permisos
    uint64 a = addr;
    for (int i = 0; i < len; i++) {
        pte_t *pte = walk(myproc()->pagetable, a + i * PGSIZE, 0);
        if (pte == 0 || (*pte & PTE_V) == 0)
            return -1;
        *pte &= ~PTE_W; // Desactivar bit de escritura
    }

    // Flush TLB para que funcionen los cambios
    sfence_vma();
    return 0;
    }
    ```

    ***Función `sys_munprotect`*** 

    - Esta función reactiva el permiso de escritura en las páginas especificadas.

    ```c
    uint64 sys_munprotect(void) {
    uint64 addr;
    int len;

    if (argaddr(0, &addr) < 0 || argint(1, &len) < 0) // Usando uint64 en lugar de void*
        return -1;

    // Validar dirección y longitud
    if (validate_addr_len((void *)addr, len, myproc()) < 0)
        return -1;

    // Recorre las páginas y modifica los permisos
    uint64 a = addr;
    for (int i = 0; i < len; i++) {
        pte_t *pte = walk(myproc()->pagetable, a + i * PGSIZE, 0);
        if (pte == 0 || (*pte & PTE_V) == 0)
            return -1;
        *pte |= PTE_W; // Activar bit de escritura
    }

    // Flush TLB para que funcionen los cambios
    sfence_vma();
    return 0;
    }
    ```
   


4. **Modificación en Syscall.h**
    -Se agregaron los identificadores de las nuevas llamadas al sistema en el archivo syscall.h:
    ```h
    #define SYS_mprotect 26
    #define SYS_munprotect 27

    ```

5. **Modificación en Syscall.c**
    -Se añadieron las funciones sys_getpriority y sys_getboost para manejar las llamadas al sistema en syscall.c:
    ```c
    extern uint64 sys_mprotect(void);
    extern uint64 sys_munprotect(void);
    ```

6.  **Modificación de usys.pl**
    -Para soportar las nuevas llamadas al sistema, se añadieron las entradas `getpriority` y `getboost` en el archivo `usys.pl`:
    ```pl
    entry("mprotect");
    entry("munprotect");
    ```

7.  **Modificación de defs.h**
    -Se modificaron las definiciones de las funciones argint, argstr y argaddr en syscall.c para que utilicen tipos específicos en lugar de void. Se explicará más adelante, porque fue parte del desafio/problema, a continuacion lo modificado:
    ```h
    //syscall.c
    int argint(int, int *);
    int argstr(int, char *, int);
    int argaddr(int, uint64 *);


8. **Compilación y Ejecución**:
   - Después de realizar los cambios, se utilizó el siguiente comando para limpiar y compilar el proyecto:
     ```bash
     make clean
     make
     ```
   - Para ejecutar el proyecto en QEMU:
     ```bash
     make qemu
     ```

6. **Ejecutar el Programa**:
   - Una vez en la consola de `xv6`, se ejecuta el programa de prueba con el siguiente comando:
     ```bash
     mprotect_test
     ```

## 4. Problemas Encontrados y Soluciones

- **Problema/Desafio: Problema de Conflicto de Tipos en la Implementación de Protección de Memoria**:
   - El problema surgió debido a conflictos de tipos al implementar las nuevas funciones de protección de memoria (mprotect y munprotect). Al usar void * en funciones como argaddr, el compilador generaba errores porque no se podía manejar correctamente el tipo de datos en operaciones específicas de dirección de memoria. La solución fue redefinir argaddr para aceptar un puntero de tipo uint64 *, ajustando los tipos de datos para que coincidieran con las operaciones requeridas en el sistema de memoria de xv6. Esto resolvió los errores de tipo y permitió una gestión adecuada de las direcciones en las nuevas syscalls.
   
## 5. Conclusión

   - En este trabajo, se implementó y depuró la funcionalidad de protección de memoria en el sistema operativo xv6, añadiendo las llamadas al sistema mprotect y munprotect. Durante el proceso, se identificaron y solucionaron problemas de compatibilidad de tipos en las funciones de obtención de argumentos de las llamadas al sistema, lo que implicó ajustes en las definiciones y firmas de funciones para garantizar que el código compilara y ejecutara correctamente. Esta implementación no solo permite una mayor seguridad en la gestión de memoria, sino que también refuerza la comprensión de la manipulación de tablas de páginas y la estructura interna de un sistema operativo. En conclusión, el desarrollo y solución de este problema proporcionaron una experiencia enriquecedora en la implementación de seguridad de memoria en un entorno de bajo nivel.
