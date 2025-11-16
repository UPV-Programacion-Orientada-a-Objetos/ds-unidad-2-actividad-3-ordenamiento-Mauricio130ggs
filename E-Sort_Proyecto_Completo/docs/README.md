# Sistema de Ordenamiento Externo E-Sort

## Descripción del Proyecto

E-Sort es un sistema de ordenamiento externo implementado en C++ que simula el procesamiento de datos masivos de telemetría en tiempo real. El sistema está diseñado para manejar flujos de datos que exceden la capacidad de la RAM disponible, utilizando técnicas de ordenamiento externo (External Mergesort).

## Características Principales

✅ **Ordenamiento Externo K-Way Merge**: Implementación completa del algoritmo Mergesort externo  
✅ **Lista Circular Doblemente Enlazada**: Buffer de memoria fijo sin uso de STL  
✅ **Programación Orientada a Objetos**: Uso de herencia, polimorfismo y clases abstractas  
✅ **Gestión Manual de Memoria**: Control total con new/delete sin fugas de memoria  
✅ **Soporte para Puerto Serial**: Lectura de datos desde Arduino (POSIX/Linux)  
✅ **Sin Dependencias de STL**: Implementación completamente manual  

## Arquitectura del Sistema

### Jerarquía de Clases

```
DataSource (Clase Abstracta)
    ├── SerialSource (Lee del puerto serial/Arduino)
    └── FileSource (Lee de archivos .tmp)

CircularBuffer (Lista Circular Doblemente Enlazada)
    └── Nodo (Estructura con punteros next/prev)

ExternalSorter (Controlador Principal)
    ├── Fase 1: Adquisición y Segmentación
    └── Fase 2: Fusión Externa (K-Way Merge)
```

### Diagrama de Flujo

```
[Arduino/Archivo] 
       ↓
[SerialSource/FileSource]
       ↓
[CircularBuffer - Tamaño Fijo]
       ↓
[Buffer Lleno?] → Sí → [Ordenar Internamente]
       ↓                       ↓
       No                 [Escribir chunk_X.tmp]
       ↓                       ↓
   [Continuar]            [Limpiar Buffer]
       ↓                       ↓
[¿Más Datos?] ────────────────┘
       ↓ No
[K-Way Merge]
   ↓    ↓    ↓
chunk_0 chunk_1 ... chunk_K
       ↓
[output.sorted.txt]
```

## Fases del Algoritmo

### Fase 1: Adquisición y Segmentación

1. Lee datos del origen (Serial o Archivo)
2. Inserta en CircularBuffer de tamaño fijo
3. Cuando el buffer está lleno:
   - Ordena internamente (Insertion Sort)
   - Escribe a `chunk_X.tmp`
   - Limpia el buffer
4. Repite hasta procesar todos los datos

### Fase 2: Fusión Externa (K-Way Merge)

1. Abre K archivos chunk simultáneamente
2. Lee el primer elemento de cada archivo
3. Encuentra el mínimo entre los K valores
4. Escribe el mínimo al archivo de salida
5. Avanza solo en el archivo del cual se extrajo el mínimo
6. Repite hasta agotar todos los archivos

## Compilación

### Requisitos

- Compilador C++11 o superior (g++, clang++)
- Make (opcional, pero recomendado)
- Sistema operativo: Linux, macOS o Windows

### Compilar el Proyecto

```bash
# Compilar todo el proyecto
make

# O compilar manualmente
g++ -Wall -std=c++11 -g -c main.cpp
g++ -Wall -std=c++11 -g -c esort.cpp
g++ -Wall -std=c++11 -g -c external_sorter.cpp
g++ -Wall -std=c++11 -g -o esort main.o esort.o external_sorter.o
```

## Uso

### Ejecución Básica

```bash
# Ejecutar con datos de prueba (por defecto)
./esort

# O usar make
make run
```

### Ejecución con Arduino Real

Para usar con un Arduino conectado al puerto serial, edita `main.cpp`:

```cpp
// Descomentar estas líneas en main.cpp
const char* SERIAL_PORT = "/dev/ttyUSB0";  // Linux
// const char* SERIAL_PORT = "COM3";       // Windows

SerialSource* serialSource = new SerialSource(SERIAL_PORT);
sorter->run(serialSource, OUTPUT_FILE);
```

### Simulador de Arduino

```bash
# Compilar el simulador
make arduino_simulator

# Ejecutar el simulador
./arduino_simulator

# O usar make
make run-simulator
```

## Comandos Make Disponibles

```bash
make              # Compilar todo
make run          # Ejecutar E-Sort
make run-simulator # Ejecutar simulador de Arduino
make clean        # Limpiar todo
make clean-temp   # Limpiar solo archivos temporales
make show-output  # Mostrar archivo de salida
make verify       # Verificar que el ordenamiento sea correcto
make help         # Mostrar ayuda
```

## Pruebas

### Prueba Básica (30 elementos)

```bash
./esort
```

Ejecuta con 30 elementos predefinidos y buffer de 1000 elementos (1 chunk).

### Prueba Extendida (50 elementos, múltiples chunks)

```bash
g++ -o test_extended test_extended.cpp esort.o external_sorter.o
./test_extended
```

Ejecuta con 50 elementos aleatorios y buffer de 10 elementos (5 chunks).

### Verificación

```bash
make verify
```

Verifica que `output.sorted.txt` esté correctamente ordenado.

## Estructura de Archivos

```
.
├── esort.h                  # Definiciones de clases
├── esort.cpp                # Implementación de DataSource, FileSource, CircularBuffer
├── external_sorter.cpp      # Implementación de ExternalSorter
├── main.cpp                 # Programa principal
├── arduino_simulator.cpp    # Simulador de Arduino
├── test_extended.cpp        # Prueba extendida
├── Makefile                 # Script de compilación
└── README.md                # Este archivo
```

## Archivos Generados

- `chunk_0.tmp`, `chunk_1.tmp`, ... - Archivos temporales ordenados
- `output.sorted.txt` - Archivo final con todos los datos ordenados
- `test_input.txt` - Archivo de prueba generado automáticamente

## Configuración

### Tamaño del Buffer

En `main.cpp` o `test_extended.cpp`:

```cpp
const int BUFFER_SIZE = 1000;  // Ajustar según necesidad
```

- **Buffer grande**: Menos chunks, menos operaciones de I/O
- **Buffer pequeño**: Más chunks, más fusiones (útil para pruebas)

### Puerto Serial

En `main.cpp`:

```cpp
const char* SERIAL_PORT = "/dev/ttyUSB0";  // Linux
const char* SERIAL_PORT = "/dev/ttyACM0";  // Arduino en Linux
const char* SERIAL_PORT = "COM3";          // Windows
```

## Conceptos Técnicos Implementados

### 1. Polimorfismo y Herencia

```cpp
DataSource* source;
source = new SerialSource("COM3");  // O
source = new FileSource("data.txt");
```

### 2. Lista Circular Doblemente Enlazada

```
     ┌─────────────────┐
     ↓                 │
[Nodo] ⇄ [Nodo] ⇄ [Nodo]
```

### 3. K-Way Merge

```
chunk_0: 1  5  9  13
chunk_1: 2  6  10 14
chunk_2: 3  7  11 15
         ↓
output:  1  2  3  5  6  7  9  10  11  13  14  15
```

### 4. Gestión de Memoria

- Destructores virtuales para polimorfismo
- Liberación explícita con `delete`
- Sin fugas de memoria (verificable con valgrind)

## Complejidad

- **Tiempo**: O(N log K) donde N = total de elementos, K = número de chunks
- **Espacio**: O(B) donde B = tamaño del buffer (constante)
- **I/O**: O(N) lecturas + O(N) escrituras

## Limitaciones Conocidas

- Soporte serial completo solo en Linux/POSIX
- Windows requiere configuración adicional para serial
- El simulador de Arduino genera datos aleatorios (no lee sensores reales)

## Posibles Mejoras

- [ ] Implementar heap (min-heap) para K-Way Merge más eficiente
- [ ] Soporte completo para Windows serial (Win32 API)
- [ ] Interfaz de configuración por línea de comandos
- [ ] Compresión de archivos temporales
- [ ] Soporte para múltiples tipos de datos (float, double, strings)

## Autor

Proyecto desarrollado como caso de estudio para:
- Estructuras de Datos Avanzadas
- Programación Orientada a Objetos
- Algoritmos de Ordenamiento Externo

## Licencia

Proyecto educativo - Uso libre para fines académicos

---

**Universidad Politécnica de Victoria**  
*Ingeniería en Tecnologías de la Información*
