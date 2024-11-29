# Sistema de Permisos Básicos e Inmutabilidad en xv6 (RISC-V)

## **Objetivo**
Desarrollar un sistema de permisos para xv6 que permita:
1. Definir permisos de solo lectura o lectura/escritura para archivos.
2. Agregar un permiso especial que marque un archivo como inmutable.

## **Detalles**
El sistema actual de xv6 permite abrir archivos en modos de lectura, escritura o ambos. Sin embargo, no restringe estas operaciones según permisos asignados al archivo. Este proyecto implementa dicha funcionalidad y añade un nuevo permiso especial de inmutabilidad.

## **Modificaciones Realizadas**

### **1. Modificación de la estructura `inode`**
Se añadió un nuevo campo al `struct inode`:
- `perm`: Define los permisos del archivo.  
  - `0`: Sin permisos.  
  - `1`: Solo lectura.  
  - `2`: Solo escritura.  
  - `3`: Lectura y escritura (valor por defecto).  
  - `5`: Archivo inmutable (nuevo permiso especial).

---

### **2. Implementación de restricciones por permisos**
Se modificaron las operaciones de apertura, lectura y escritura para verificar los permisos definidos en el `inode`:
- **Apertura (`sys_open`)**: 
  - Si un archivo es de solo lectura (`1`), no se puede abrir en modo de escritura.
  - Si es inmutable (`5`), solo puede abrirse en modo lectura.
- **Escritura (`writei`)**:
  - No se permite si el archivo tiene permisos de solo lectura o es inmutable.
- **Cambio de permisos (`chmod`)**:
  - Cambia los permisos del archivo. Si el archivo es inmutable, esta operación falla.

---

### **3. Implementación de la llamada al sistema `chmod`**
Se agregó una nueva llamada al sistema:
- **Prototipo**: `int chmod(char *filename, int mode)`
- **Funcionalidad**:
  - Cambia los permisos del archivo especificado a `mode`.
  - Si el archivo es inmutable (`perm = 5`), devuelve un error.

---

### **4. Programa de prueba**
Se creó un programa en espacio de usuario (`chmod_test.c`) que valida las nuevas funcionalidades:
- **Creación del archivo**:
  - Crea un archivo con permisos de lectura/escritura (`O_CREATE | O_RDWR`).
  - Escribe datos en el archivo y lo cierra.
- **Cambio de permisos a solo lectura**:
  - Cambia los permisos con `chmod(filename, 1)`.
  - Verifica que el archivo no puede abrirse en modo de escritura.
- **Restauración de permisos a lectura/escritura**:
  - Cambia los permisos con `chmod(filename, 3)` y escribe nuevamente.
- **Prueba de inmutabilidad**:
  - Cambia los permisos del archivo a inmutable (`chmod(filename, 5)`).
  - Verifica que:
    - No se pueda escribir en el archivo.
    - No se puedan cambiar los permisos.
---

##  Archivos Modificados y Creaciones Nuevas


1. **Archivo Creado: `user/chmod_test.c`**:

    -Este programa, escrito para el sistema operativo xv6, realiza pruebas para validar la funcionalidad de un sistema de permisos implementado en los archivos del sistema de archivos. A continuación, se describe el funcionamiento del código:

2. **Creación del archivo**:  
   - Se utiliza la función `open` con las banderas `O_CREATE | O_RDWR` para crear un archivo llamado `testfile` con permisos de lectura y escritura.
   - Si la creación falla, el programa imprime un mensaje de error y termina su ejecución.
   - Se escribe el texto `"hola"` en el archivo, y luego se cierra con `close`.

3. **Cambio de permisos a solo lectura**:  
   - Se usa la función `chmod` para cambiar los permisos del archivo a solo lectura (`1`).
   - Posteriormente, el programa intenta abrir el archivo en modo escritura (`O_WRONLY`).
   - Si la apertura falla, se imprime un mensaje confirmando que los permisos de solo lectura fueron respetados. Si no falla, se notifica un error porque el archivo no debería ser accesible en modo escritura.

4. **Marcado como inmutable**:  
   - Se vuelve a usar `chmod` para marcar el archivo como inmutable (`5`), lo que implica que no se puede modificar ni cambiar sus permisos.
   - Se realiza un intento de modificar los permisos del archivo a lectura/escritura (`3`) usando `chmod`. Como el archivo es inmutable, la operación falla y el programa imprime un mensaje confirmando que los permisos no se pudieron cambiar.

5. **Eliminación del archivo**:  
   - Finalmente, el archivo se elimina usando `unlink` y el programa termina.

En sintesis, este programa verifica que:
1. Los permisos de solo lectura restringen correctamente las operaciones de escritura.
2. Los archivos marcados como inmutables no permiten cambios en sus permisos, asegurando la integridad del sistema de archivos.

   
   ### Código del archivo `chmod_test.c`:

    ```c
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

    ```

6. **Modificación en `Makefile`**:
   - Para que el programa `chmod_test.c` sea compilado y ejecutado como un programa de usuario en `xv6`, fue necesario agregarlo al `Makefile` de la siguiente manera:
     - Se añadió la línea `$U/_chmod_test\` en la sección `UPROGS`:
     ```makefile
     UPROGS=\
     	$U/_cat\
     	$U/_echo\
     	...
     	$U/_chmod_test\
     ```

7. **Modificación en Syscall.h**
    -Se agregaron los identificadores de las nuevas llamadas al sistema en el archivo syscall.h:
    
    ```h
    #define SYS_chmod 28
    ```

8. **Modificación en Syscall.c**
    ```c
    extern uint64 sys_chmod(void);
    ```

9.  **Modificación de usys.pl**
    -Para soportar las nuevas llamadas al sistema, se añadieron en el archivo `usys.pl`:
    ```pl
    entry("chmod");
    ```


10. **Compilación y Ejecución**:
   - Después de realizar los cambios, se utilizó el siguiente comando para limpiar y compilar el proyecto:
     ```bash
     make clean
     make
     ```
   - Para ejecutar el proyecto en QEMU:
     ```bash
     make qemu
     ```

11. **Ejecutar el Programa**:
   - Una vez en la consola de `xv6`, se ejecuta el programa de prueba con el siguiente comando:
     ```bash
     chmod_test
     ```

### Problemas Detectados

Un inconveniente surgió al modificar la estructura `dinode` para incluir el campo `perm`, ya que esto alteró su tamaño, impidiendo que fuera divisible por el tamaño del bloque (`BSIZE`). Esta desalineación generó errores durante la inicialización del sistema de archivos, ya que la estructura no cumplía con los requisitos de alineación del sistema. Para solucionar este problema, se añadió un campo de relleno (padding) a la estructura, asegurando que su tamaño final sea múltiplo de `BSIZE`, lo que permitió restaurar la compatibilidad y corregir los errores.

### Conclusión

La implementación de un sistema de permisos básicos en xv6 permitió extender las funcionalidades del sistema operativo para manejar archivos con restricciones más específicas, como acceso de solo lectura y un estado inmutable. Esto involucró modificaciones en la estructura de datos `inode`, la introducción de una nueva llamada al sistema `chmod` y ajustes en las operaciones de apertura, lectura y escritura para respetar los nuevos permisos. A pesar de los desafíos técnicos, como la desalineación en el tamaño del `dinode` y el manejo de permisos inmutables, las soluciones implementadas garantizaron un funcionamiento correcto y cumplieron con los objetivos planteados. Este proyecto no solo refuerza el entendimiento sobre la gestión de sistemas de archivos, sino que también destaca la importancia de mantener la coherencia entre las estructuras internas y las funcionalidades del sistema operativo.
