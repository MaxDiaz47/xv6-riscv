# Proyecto XV6 - Tarea 2

Este documento describe los pasos realizados para completar la tarea 2 de sistemas operativos, concretamente, `xv6-riscv`, junto con los cambios realizados en el código fuente, los archivos creados, y cómo se ejecuta el proyecto.

## 1. Descripción del Proyecto

En esta tarea, se debía modificar el sistema operativo `xv6-riscv` para crear un programa que ejecutara múltiples procesos hijos, imprimiera el PID de cada uno de ellos y sincronizara las salidas para evitar desorden en la consola. A continuación, se describen los pasos seguidos y las modificaciones hechas en el código de `xv6-riscv`.


## 2. Archivos Modificados y Creaciones Nuevas

1. **Archivo Creado: `user/proceso_test.c`**:
   - Se creó el archivo `proceso_test.c` dentro de la carpeta `user/`, que contiene el programa encargado de crear 20 procesos hijos, imprimir su PID y sincronizar la salida para evitar desorden.
   
   ### Código del archivo `proceso_test.c`:

    ```c
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
        
        int priority = getpriority(); // Llamada al sistema para obtener la prioridad
        int boost = getboost();       // Llamada al sistema para obtener el boost

        itoa(priority, buffer);
        write(1, " con Prioridad: ", 16);
        write(1, buffer, strlen(buffer));  // Escribir la prioridad

        itoa(boost, buffer);
        write(1, " y Boost: ", 10);
        write(1, buffer, strlen(buffer));  // Escribir el boost
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
    ```

2. **Modificación en `Makefile`**:
   - Para que el programa `proceso_test.c` sea compilado y ejecutado como un programa de usuario en `xv6`, fue necesario agregarlo al `Makefile` de la siguiente manera:
     - Se añadió la línea `$U/_proceso_test\` en la sección `UPROGS`:
     ```makefile
     UPROGS=\
     	$U/_cat\
     	$U/_echo\
     	...
     	$U/_proceso_test\
     ```

3. **Modificación en `proc.h`**:
   - Se agregaron dos nuevos campos a la estructura `proc` para manejar la prioridad y el boost de los procesos:
     ```c
     int priority;  // Nueva prioridad del proceso
     int boost;     // Boost del proceso
     ```

4. **Modificación en `proc.c`**:
   - En la función `allocproc()`, se inicializaron los campos `priority` y `boost` cuando se asigna un nuevo proceso:
     ```c
     p->priority = 0;  // Inicializa la prioridad a 0
     p->boost = 1;     // Inicializa el boost a 1
     ```
      - También se agregó código en el scheduler para ajustar el `boost` según la prioridad de los procesos:
     ```c
     p->priority += p->boost;  // Incrementar o disminuir la prioridad según el boost

     // Si la prioridad llega al máximo, cambia el boost para disminuir la prioridad
     if (p->priority >= 9) {
         p->boost = -1;
     }

     // Si la prioridad llega al mínimo, cambia el boost para aumentarla
     if (p->priority <= 0) {
         p->boost = 1;
     }
     ```
5. **Modificación en Syscall.h**
    -Se agregaron los identificadores de las nuevas llamadas al sistema en el archivo syscall.h:
    ```
    #define SYS_getpriority 23
    #define SYS_getboost 24
    ```

6. **Modificación en Syscall.c**
    -Se añadieron las funciones sys_getpriority y sys_getboost para manejar las llamadas al sistema en syscall.c:
    ```
    extern uint64 sys_getpriority(void);
    extern uint64 sys_getboost(void);
    ```

7.  **Modificación de usys.pl**
    -Para soportar las nuevas llamadas al sistema, se añadieron las entradas `getpriority` y `getboost` en el archivo `usys.pl`:
    ```
    entry("getpriority");
    entry("getboost");
    ```

8.  **Modificación de user.h**
    -Se añadieron las definiciones de las llamadas al sistema `getpriority` y `getboost` en el archivo `user.h` para que puedan ser utilizadas en los programas de usuario:
    ```
    int getpriority(void);  
    int getboost(void);
    ```
    

5. **Compilación y Ejecución**:
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
     proceso_test
     ```

## 4. Problemas Encontrados y Soluciones

- **Problema 1: Impresión Desordenada o con Caracteres Extraños**:
   - Se intentó usar `printf()` inicialmente, pero causaba problemas de sincronización en la salida de los procesos. Para solucionar esto, se usó la función `write()` y se implementó una pausa con `sleep()` para evitar que los procesos imprimieran al mismo tiempo.
   
- **Problema 2: Uso de `itoa()` en Lugar de `snprintf()`**:
   - Durante la compilación, `snprintf()` no estaba disponible en `xv6`. Como alternativa, se implementó la función `itoa()` para convertir el PID de los procesos a cadenas de texto. Esta función convierte el entero a una representación de cadena que luego se imprime utilizando `write()`.
## 5. Conclusión

Este proyecto nos permitió implementar un sistema de sincronización simple entre múltiples procesos dentro del sistema operativo `xv6`. Se lograron evitar problemas comunes de concurrencia relacionados con la salida estándar, y se pudieron ejecutar 20 procesos hijos de manera controlada.
