# Documentación Técnica - Sistema E-Sort

## Tabla de Contenidos

1. [Introducción](#introducción)
2. [Conceptos Teóricos](#conceptos-teóricos)
3. [Diseño e Implementación](#diseño-e-implementación)
4. [Análisis de Complejidad](#análisis-de-complejidad)
5. [Decisiones de Diseño](#decisiones-de-diseño)
6. [Casos de Uso](#casos-de-uso)

---

## Introducción

E-Sort es una implementación educativa de un sistema de ordenamiento externo que demuestra cómo procesar conjuntos de datos que exceden la memoria RAM disponible. El sistema utiliza el algoritmo **External Mergesort** con una variante **K-Way Merge**.

### Problema a Resolver

Un laboratorio genera datos de telemetría a una velocidad que excede la capacidad de RAM. Los requisitos son:

- ✅ Ordenar completamente los datos
- ✅ Usar memoria constante (buffer fijo)
- ✅ No perder ningún dato
- ✅ Implementación manual (sin STL)

---

## Conceptos Teóricos

### 1. Ordenamiento Externo

**Definición**: Algoritmos de ordenamiento diseñados para datos que no caben en memoria principal y deben residir en almacenamiento secundario (disco).

**Características**:
- Divide los datos en "chunks" que caben en RAM
- Ordena cada chunk individualmente
- Fusiona los chunks ordenados

**Ventajas**:
- Maneja conjuntos de datos arbitrariamente grandes
- Uso de memoria predecible y constante
- Escalabilidad

### 2. Mergesort Externo

**Algoritmo en dos fases**:

#### Fase 1: División y Ordenamiento
```
Mientras hay datos:
    1. Llenar buffer de tamaño B
    2. Ordenar buffer en memoria
    3. Escribir a chunk_i.tmp
    4. Limpiar buffer
```

#### Fase 2: Fusión (K-Way Merge)
```
1. Abrir K archivos chunk
2. Leer primer elemento de cada archivo
3. Mientras existan elementos:
    a. Encontrar mínimo entre K valores
    b. Escribir mínimo a salida
    c. Avanzar solo en el archivo del mínimo
```

### 3. K-Way Merge Explicado

**Ejemplo con K=3 chunks**:

```
chunk_0:  [1, 5, 9, 13]
chunk_1:  [2, 6, 10, 14]
chunk_2:  [3, 7, 11, 15]

Iteración 1: min(1, 2, 3) = 1 → output: [1]
                ↑
Iteración 2: min(5, 2, 3) = 2 → output: [1, 2]
                   ↑
Iteración 3: min(5, 6, 3) = 3 → output: [1, 2, 3]
                      ↑
Iteración 4: min(5, 6, 7) = 5 → output: [1, 2, 3, 5]
                ↑
...y así sucesivamente
```

### 4. Lista Circular Doblemente Enlazada

**Estructura**:

```
        head
         ↓
    ┌────────────────┐
    ↓                │
[Node] ⇄ [Node] ⇄ [Node]
  │         │         │
data=5   data=10   data=15
```

**Ventajas**:
- Inserción O(1) al final
- Recorrido bidireccional
- Implementación natural de buffer circular

**Implementación**:

```cpp
struct Node {
    int data;
    Node* next;
    Node* prev;
};
```

---

## Diseño e Implementación

### Jerarquía de Clases (POO)

#### 1. Clase Base Abstracta: DataSource

```cpp
class DataSource {
public:
    virtual int getNext() = 0;
    virtual bool hasMoreData() = 0;
    virtual ~DataSource() {}
};
```

**Propósito**: Define una interfaz común para diferentes fuentes de datos.

**Ventajas del Polimorfismo**:
- Código genérico que funciona con cualquier fuente
- Fácil extensión (agregar NetworkSource, DatabaseSource, etc.)
- Separación de responsabilidades

#### 2. SerialSource (Lee del Arduino)

```cpp
class SerialSource : public DataSource {
    int fileDescriptor;  // POSIX file descriptor
    bool connected;
    
public:
    SerialSource(const char* portName);
    int getNext() override;
    bool hasMoreData() override;
};
```

**Detalles de Implementación**:

```cpp
// Configuración del puerto serial (POSIX)
struct termios tty;
cfsetospeed(&tty, B9600);  // 9600 baud
tty.c_cflag = CS8;         // 8 bits
tty.c_cflag &= ~PARENB;    // Sin paridad
```

#### 3. FileSource (Lee de archivos chunk)

```cpp
class FileSource : public DataSource {
    std::ifstream file;
    bool isOpen;
    
public:
    FileSource(const char* filename);
    int getNext() override;
    bool hasMoreData() override;
};
```

**Gestión de EOF**:

```cpp
bool FileSource::hasMoreData() {
    file >> ws;  // Saltar whitespace
    return file.good() && file.peek() != EOF;
}
```

#### 4. CircularBuffer

```cpp
class CircularBuffer {
    Node* head;
    int capacity;
    int currentSize;
    
public:
    bool insert(int value);
    void sort();  // Insertion Sort
    void writeToFile(const char* filename);
    void clear();
};
```

**Insertion Sort en Lista Circular**:

```cpp
void CircularBuffer::sort() {
    // Construir nueva lista ordenada
    Node* sorted = nullptr;
    
    for cada nodo actual {
        // Encontrar posición de inserción
        while (valor_actual > valor_en_posición) {
            avanzar posición;
        }
        // Insertar en posición correcta
        enlazar nodo actual;
    }
    
    head = sorted;
}
```

**Complejidad**: O(n²) pero n es pequeño (tamaño del buffer)

#### 5. ExternalSorter (Controlador)

```cpp
class ExternalSorter {
    CircularBuffer* buffer;
    int chunkCount;
    
    void phase1_Acquisition(DataSource* source);
    void phase2_Merge(int numChunks, const char* output);
    
public:
    void run(DataSource* source, const char* output);
};
```

### Fase 1: Implementación Detallada

```cpp
void ExternalSorter::phase1_Acquisition(DataSource* source) {
    while (source->hasMoreData()) {
        int value = source->getNext();
        
        if (!buffer->insert(value)) {
            // Buffer lleno → procesar
            buffer->sort();                    // O(B²)
            buffer->writeToFile("chunk_X");    // O(B)
            buffer->clear();                   // O(B)
            chunkCount++;
            
            buffer->insert(value);  // Insertar valor actual
        }
    }
    
    // Procesar datos restantes
    if (buffer->getSize() > 0) {
        buffer->sort();
        buffer->writeToFile("chunk_X");
        chunkCount++;
    }
}
```

### Fase 2: K-Way Merge Implementación

```cpp
void ExternalSorter::phase2_Merge(int K, const char* output) {
    DataSource** sources = new DataSource*[K];
    int* currentValues = new int[K];
    bool* hasValidValue = new bool[K];
    
    // Inicializar: leer primer elemento de cada chunk
    for (int i = 0; i < K; i++) {
        sources[i] = new FileSource("chunk_i.tmp");
        if (sources[i]->hasMoreData()) {
            currentValues[i] = sources[i]->getNext();
            hasValidValue[i] = true;
        }
    }
    
    ofstream output_file(output);
    
    // Loop principal de fusión
    while (true) {
        // Encontrar mínimo
        int minIndex = findMinimum(currentValues, hasValidValue, K);
        
        if (minIndex == -1) break;  // No más datos
        
        // Escribir mínimo
        output_file << currentValues[minIndex] << "\n";
        
        // Avanzar en el archivo del mínimo
        if (sources[minIndex]->hasMoreData()) {
            currentValues[minIndex] = sources[minIndex]->getNext();
        } else {
            hasValidValue[minIndex] = false;
        }
    }
    
    // Cleanup
    for (int i = 0; i < K; i++) {
        delete sources[i];
    }
    delete[] sources;
    delete[] currentValues;
    delete[] hasValidValue;
}
```

**Búsqueda de Mínimo (Lineal)**:

```cpp
int findMinimum(int* values, bool* active, int K) {
    int minIndex = -1;
    
    for (int i = 0; i < K; i++) {
        if (active[i]) {
            if (minIndex == -1 || values[i] < values[minIndex]) {
                minIndex = i;
            }
        }
    }
    
    return minIndex;
}
```

---

## Análisis de Complejidad

### Notación

- **N**: Total de elementos a ordenar
- **B**: Tamaño del buffer (capacidad)
- **K**: Número de chunks = ⌈N/B⌉

### Fase 1: Adquisición

| Operación | Complejidad | Frecuencia | Total |
|-----------|-------------|------------|-------|
| Insertar en buffer | O(1) | N veces | O(N) |
| Ordenar buffer | O(B²) | K veces | O(K·B²) |
| Escribir chunk | O(B) | K veces | O(K·B) |

**Complejidad Fase 1**: O(N + K·B²)

Dado que K = N/B:
- O(N + (N/B)·B²) = O(N + N·B) = **O(N·B)**

### Fase 2: K-Way Merge

| Operación | Complejidad | Frecuencia | Total |
|-----------|-------------|------------|-------|
| Buscar mínimo | O(K) | N veces | O(N·K) |
| Leer de archivo | O(1) | N veces | O(N) |
| Escribir a salida | O(1) | N veces | O(N) |

**Complejidad Fase 2**: O(N·K)

Dado que K = N/B:
- O(N · N/B) = **O(N²/B)**

### Complejidad Total

**Tiempo**: O(N·B + N²/B)

**Optimización del Buffer**:
- Para minimizar: derivar respecto a B e igualar a 0
- Resultado: B_óptimo = √N

**Con B óptimo**: O(N√N) ≈ O(N^1.5)

### Complejidad Espacial

- **Buffer**: O(B) = constante
- **Arrays temporales en merge**: O(K) = O(N/B)
- **Espacio en disco**: O(N) para chunks + O(N) para salida = O(N)

**Espacio en RAM**: **O(B)** = constante y configurable

---

## Decisiones de Diseño

### 1. ¿Por qué Lista Circular en lugar de Array?

**Array**:
```cpp
int buffer[1000];  // Fijo, desperdicia memoria
```

**Lista Circular**:
```cpp
Node* buffer;  // Dinámica, solo usa lo necesario
```

**Ventajas de Lista Circular**:
- Insertiones O(1) al final
- Fácil de limpiar (recorrer y delete)
- Demostrativo de estructuras dinámicas

**Desventaja**:
- Ordenamiento O(n²) vs O(n log n) posible con array

### 2. ¿Por qué Insertion Sort en lugar de Mergesort?

Para el buffer interno:

**Insertion Sort**:
- O(B²) pero B es pequeño (≈1000)
- Implementación simple sobre lista enlazada
- Mejor para datos casi ordenados

**Mergesort Recursivo**:
- O(B log B) asintóticamente mejor
- Más complejo de implementar sobre lista
- Overhead no justificado para B pequeño

**Decisión**: Insertion Sort es suficiente y más simple.

### 3. ¿Por qué Búsqueda Lineal del Mínimo?

**Alternativa**: Min-Heap

```cpp
// Con heap
priority_queue<pair<int, int>> minHeap;  // (valor, índice_chunk)
// O(log K) para encontrar mínimo
```

**Decisión Actual**: Búsqueda lineal O(K)

**Justificación**:
- K típicamente pequeño (< 100)
- Implementación más simple
- Evita complejidad adicional
- Para K grande, heap sería mejor

### 4. Gestión de Memoria Manual

**Sin STL**:
```cpp
DataSource** sources = new DataSource*[K];
// ...
delete[] sources;
```

**Con STL**:
```cpp
std::vector<std::unique_ptr<DataSource>> sources;
// Auto-cleanup
```

**Justificación de Decisión**:
- Requisito educativo (demostrar gestión manual)
- Control total sobre memoria
- Entendimiento profundo de punteros

**Precauciones Implementadas**:
- Destructores virtuales
- Delete en orden correcto
- RAII donde es posible

---

## Casos de Uso

### Caso 1: Análisis de Logs de Servidor

**Escenario**: 100 GB de logs web desordenados

**Configuración**:
```cpp
const int BUFFER_SIZE = 100000;  // 100K líneas
FileSource* logs = new FileSource("access.log");
sorter->run(logs, "sorted_logs.txt");
```

**Resultado**: Logs ordenados por timestamp

### Caso 2: Procesamiento de Sensores IoT

**Escenario**: Miles de sensores enviando lecturas

**Configuración**:
```cpp
const int BUFFER_SIZE = 5000;
SerialSource* sensor = new SerialSource("/dev/ttyUSB0");
sorter->run(sensor, "sensor_data_sorted.txt");
```

**Resultado**: Datos de sensores ordenados para análisis

### Caso 3: Ordenamiento de IDs de Usuario

**Escenario**: Base de datos con millones de user_ids desordenados

**Configuración**:
```cpp
const int BUFFER_SIZE = 50000;
FileSource* userIds = new FileSource("user_ids.txt");
sorter->run(userIds, "sorted_user_ids.txt");
```

**Resultado**: IDs ordenados para búsqueda binaria

---

## Mejoras Futuras

### 1. Implementar Min-Heap para Merge

**Beneficio**: O(log K) vs O(K) actual

```cpp
#include <queue>

priority_queue<
    pair<int, int>,           // (valor, índice)
    vector<pair<int, int>>,
    greater<pair<int, int>>   // Min-heap
> minHeap;
```

### 2. Merge Multi-nivel

Para K muy grande (> 100):

```
K=1000 chunks → Merge de 10 en 10
    Nivel 1: 1000 → 100 chunks
    Nivel 2: 100 → 10 chunks
    Nivel 3: 10 → 1 chunk final
```

### 3. Compresión de Chunks

```cpp
// Escribir chunks comprimidos
void writeCompressed(const char* filename) {
    // Implementar compresión simple (RLE, etc.)
}
```

**Beneficio**: Menos I/O de disco

### 4. Paralelización

```cpp
// Fase 1: Procesar múltiples buffers en paralelo
#pragma omp parallel for
for (int i = 0; i < num_threads; i++) {
    processChunk(i);
}
```

---

## Conclusiones

Este proyecto demuestra:

✅ **Algoritmos**: Mergesort externo, K-Way Merge  
✅ **Estructuras**: Listas circulares, herencia, polimorfismo  
✅ **Sistemas**: I/O de archivos, comunicación serial  
✅ **Ingeniería**: Gestión de memoria, diseño modular  

**Aplicaciones Reales**:
- Bases de datos (external sort en SQL)
- Procesamiento de Big Data (MapReduce sort phase)
- Análisis de logs
- Procesamiento de telemetría

---

**Documento creado para el proyecto E-Sort**  
Universidad Politécnica de Victoria - ITI
