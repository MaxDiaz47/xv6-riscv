
# **Informe de Instalación de XV6 y Toolchain de RISC-V**

## **1. Instalación Inicial**

### **Paso 1: Instalación de Herramientas Básicas**
Primero, se instaló las herramientas esenciales necesarias para compilar y ejecutar XV6:

```bash
sudo apt-get update
sudo apt-get install git qemu-system-misc build-essential gdb
```
**Descripción**: 
- **git**: Para clonar el código fuente de XV6 desde GitHub.
- **qemu-system-misc**: Incluye QEMU, un emulador necesario para ejecutar XV6.
- **build-essential**: Paquete que contiene compiladores y utilidades necesarias para compilar software.
- **gdb**: Depurador que puede ser útil para analizar problemas en el código.

### **Paso 2: Clonar el Repositorio de XV6**
Ahora, se descargó el código fuente de XV6 desde GitHub:

```bash
git clone https://github.com/mit-pdos/xv6-riscv.git
cd xv6-riscv
```
**Descripción**: 
- Este comando clona el repositorio de XV6 en tu computadora y navega dentro del directorio descargado.

### **Paso 3: Compilar y Ejecutar XV6**
se compiló y se ejecutó el sistema operativo XV6:

```bash
make clean
make
make qemu
```
**Descripción**:
- **make clean**: Limpia cualquier compilación previa.
- **make**: Compila el código fuente de XV6.
- **make qemu**: Ejecuta XV6 en el emulador QEMU.
- Nota: Esto dio error, por lo que se continuó con los siguientes pasos.

### **Paso 4: Instalación de Herramientas Adicionales y Clonar la Toolchain de RISC-V**
se instaló herramientas adicionales necesarias para compilar la toolchain de RISC-V y se descargó el código fuente de la toolchain:

```bash
sudo apt-get install git qemu-system-misc build-essential gdb
git clone --recursive https://github.com/riscv/riscv-gnu-toolchain
cd riscv-gnu-toolchain
```
**Descripción**:
- El primer comando instala herramientas esenciales (ya explicadas antes).
- El segundo comando clona el repositorio de la toolchain de RISC-V, que es necesaria para compilar y ejecutar código para la arquitectura RISC-V.

### **Paso 5: Instalación de Dependencias y Compilación de la Toolchain**
Para compilar la toolchain de RISC-V, se necesitó instalar algunas dependencias adicionales y luego proceder con la compilación.

**Comando para instalar dependencias adicionales**:
```bash
sudo apt-get install autoconf automake autotools-dev curl python3 python3-pip libmpc-dev libmpfr-dev libgmp-dev gawk build-essential bison flex texinfo gperf libtool patchutils bc zlib1g-dev libexpat-dev
```
**Descripción**:
- Estas herramientas son necesarias para configurar y compilar la toolchain de RISC-V. Incluyen utilidades para el procesamiento de archivos, compiladores de varios lenguajes, y bibliotecas esenciales para la construcción del software.

**Comando para configurar y compilar la toolchain**:
```bash
./configure --prefix=/opt/riscv
sudo make -j$(nproc)
```
**Descripción**:
- **./configure --prefix=/opt/riscv**: Configura la compilación de la toolchain y define `/opt/riscv` como el directorio donde se instalará la toolchain.
- **sudo make -j$(nproc)**: Compila la toolchain utilizando múltiples núcleos de tu procesador para acelerar el proceso. Este comando puede tardar un tiempo en completarse.

### **Paso 6: Añadir la Toolchain al PATH del Sistema**
Para que el sistema pueda encontrar y utilizar la toolchain de RISC-V, es necesario añadirla al PATH.

**Comando**:
```bash
echo 'export PATH=/opt/riscv/bin:$PATH' >> ~/.bashrc
source ~/.bashrc
```
**Descripción**:
- **echo 'export PATH=/opt/riscv/bin:$PATH' >> ~/.bashrc**: Añade la ruta de la toolchain al archivo `.bashrc`, que se ejecuta automáticamente al iniciar una sesión de terminal.
- **source ~/.bashrc**: Recarga el archivo `.bashrc` para que los cambios tengan efecto inmediato.

### **Paso 7: Compilar y Ejecutar XV6 con la Nueva Toolchain**
Finalmente, se volvió al directorio de XV6, se limpió cualquier compilación previa, se compiló de nuevo utilizando la nueva toolchain de RISC-V, y se ejecutó el sistema operativo en QEMU.

**Comandos**:
```bash
cd ~/xv6-riscv
make clean
make
make qemu
```
**Descripción**:
- **cd ~/xv6-riscv**: Navega de nuevo al directorio donde está el código fuente de XV6.
- **make clean**: Limpia compilaciones anteriores.
- **make**: Compila XV6 utilizando la toolchain de RISC-V que acabas de configurar.
- **make qemu**: Ejecuta XV6 en QEMU, emulando la arquitectura RISC-V.

### Código 
$ echo hola
hola
$ ls
.              1 1 1024
..             1 1 1024
README         2 2 2403
cat            2 3 35552
echo           2 4 34408
forktest       2 5 16272
grep           2 6 38992
init           2 7 34856
kill           2 8 34336
ln             2 9 34136
ls             2 10 37664
mkdir          2 11 34392
rm             2 12 34376
sh             2 13 57120
stressfs       2 14 35256
usertests      2 15 181176
grind          2 16 50832
wc             2 17 36536
zombie         2 18 33736
console        3 19 0
$ cat README
xv6 is a re-implementation of Dennis Ritchie's and Ken Thompson's Unix
Version 6 (v6).  xv6 loosely follows the structure and style of v6,
but is implemented for a modern RISC-V multiprocessor using ANSI C.

ACKNOWLEDGMENTS

xv6 is inspired by John Lions's Commentary on UNIX 6th Edition (Peer
to Peer Communications; ISBN: 1-57398-013-7; 1st edition (June 14,
2000)).  See also https://pdos.csail.mit.edu/6.1810/, which provides
pointers to on-line resources for v6.

The following people have made contributions: Russ Cox (context switching,
locking), Cliff Frey (MP), Xiao Yu (MP), Nickolai Zeldovich, and Austin
Clements.

We are also grateful for the bug reports and patches contributed by
Takahiro Aoyagi, Marcelo Arroyo, Silas Boyd-Wickizer, Anton Burtsev,
carlclone, Ian Chen, Dan Cross, Cody Cutler, Mike CAT, Tej Chajed,
Asami Doi,Wenyang Duan, eyalz800, Nelson Elhage, Saar Ettinger, Alice
Ferrazzi, Nathaniel Filardo, flespark, Peter Froehlich, Yakir Goaron,
Shivam Handa, Matt Harvey, Bryan Henry, jaichenhengjie, Jim Huang,
Matúš Jókay, John Jolly, Alexander Kapshuk, Anders Kaseorg, kehao95,
Wolfgang Keller, Jungwoo Kim, Jonathan Kimmitt, Eddie Kohler, Vadim
Kolontsov, Austin Liew, l0stman, Pavan Maddamsetti, Imbar Marinescu,
Yandong Mao, Matan Shabtay, Hitoshi Mitake, Carmi Merimovich, Mark
Morrissey, mtasm, Joel Nider, Hayato Ohhashi, OptimisticSide,
phosphagos, Harry Porter, Greg Price, RayAndrew, Jude Rich, segfault,
Ayan Shafqat, Eldar Sehayek, Yongming Shen, Fumiya Shigemitsu, snoire,
Taojie, Cam Tenny, tyfkda, Warren Toomey, Stephen Tu, Alissa Tung,
Rafael Ubal, Amane Uehara, Pablo Ventura, Xi Wang, WaheedHafez,
Keiichi Watanabe, Lucas Wolf, Nicolas Wolovick, wxdao, Grant Wu, x653,
Jindong Zhang, Icenowy Zheng, ZhUyU1997, and Zou Chang Wei.

The code in the files that constitute xv6 is
Copyright 2006-2024 Frans Kaashoek, Robert Morris, and Russ Cox.

ERROR REPORTS

Please send errors and suggestions to Frans Kaashoek and Robert Morris
(kaashoek,rtm@mit.edu).  The main purpose of xv6 is as a teaching
operating system for MIT's 6.1810, so we are more interested in
simplifications and clarifications than new features.

BUILDING AND RUNNING XV6

You will need a RISC-V "newlib" tool chain from
https://github.com/riscv/riscv-gnu-toolchain, and qemu compiled for
riscv64-softmmu.  Once they are installed, and in your shell
search path, you can run "make qemu".
$
###