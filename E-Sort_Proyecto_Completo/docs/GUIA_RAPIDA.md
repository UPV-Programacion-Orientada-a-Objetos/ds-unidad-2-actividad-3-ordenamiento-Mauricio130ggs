# Guía Rápida de Inicio - E-Sort

## ⚡ Inicio Rápido (5 minutos)

### 1. Compilar

```bash
make
```

### 2. Ejecutar

```bash
./esort
```

### 3. Verificar

```bash
make verify
```

¡Listo! El sistema ha procesado 30 elementos de prueba.

---

## 📁 Archivos del Proyecto

### Archivos Principales (Obligatorios)

| Archivo | Descripción |
|---------|-------------|
| `esort.h` | Definiciones de todas las clases |
| `esort.cpp` | Implementación de DataSource, FileSource, CircularBuffer |
| `external_sorter.cpp` | Implementación de ExternalSorter (Fase 1 y 2) |
| `main.cpp` | Programa principal |
| `Makefile` | Script de compilación |

### Archivos Auxiliares

| Archivo | Descripción |
|---------|-------------|
| `arduino_simulator.cpp` | Simula Arduino generando datos |
| `test_extended.cpp` | Prueba con múltiples chunks |
| `arduino_telemetry.ino` | Código para cargar en Arduino real |
| `README.md` | Documentación completa |
| `DOCUMENTACION_TECNICA.md` | Explicación técnica detallada |

---

## 🎯 Casos de Prueba

### Prueba 1: Básica (1 chunk)
```bash
./esort
# 30 elementos, buffer=1000, resultado: 1 chunk
```

### Prueba 2: Múltiples Chunks
```bash
g++ -o test_extended test_extended.cpp esort.o external_sorter.o
./test_extended
# 50 elementos, buffer=10, resultado: 5 chunks
```

### Prueba 3: Con Arduino Simulado
```bash
./arduino_simulator > datos_arduino.txt &
# Editar main.cpp para leer de "datos_arduino.txt"
./esort
```

---

## 🔧 Configuración

### Cambiar Tamaño del Buffer

En `main.cpp`:
```cpp
const int BUFFER_SIZE = 1000;  // ← Cambiar aquí
```

**Recomendaciones**:
- Buffer pequeño (10-100): Genera muchos chunks, bueno para pruebas
- Buffer grande (1000+): Menos chunks, más eficiente

### Usar Arduino Real

1. Cargar `arduino_telemetry.ino` en el Arduino
2. Identificar puerto (Linux: `/dev/ttyUSB0`, Windows: `COM3`)
3. En `main.cpp`, descomentar:

```cpp
const char* SERIAL_PORT = "/dev/ttyUSB0";  // Tu puerto
SerialSource* serialSource = new SerialSource(SERIAL_PORT);
sorter->run(serialSource, OUTPUT_FILE);
```

---

## 📊 Entender la Salida

### Salida de Consola

```
========================================
Sistema de Ordenamiento Externo E-Sort
========================================
Capacidad del buffer: 1000 elementos

Iniciando Fase 1: Adquisicion de datos...

Leyendo -> 105
Leyendo -> 5
...
Buffer lleno. Ordenando internamente...
Buffer ordenado: [5, 99, 105, 210]
Escribiendo chunk_0.tmp... OK.

Fase 1 completada. 2 chunks generados.

Iniciando Fase 2: Fusion Externa (K-Way Merge)
...
Fusion completada. Total de elementos escritos: 30
```

### Archivos Generados

- `chunk_0.tmp`, `chunk_1.tmp`, ... - Archivos temporales ordenados
- `output.sorted.txt` - Resultado final (todos los datos ordenados)

---

## 🐛 Resolución de Problemas

### Error: "No se pudo abrir puerto"

**Causa**: Puerto serial no disponible o permisos insuficientes

**Solución**:
```bash
# Linux: Agregar usuario al grupo dialout
sudo usermod -a -G dialout $USER
# Reiniciar sesión

# Verificar puertos disponibles
ls /dev/tty*
```

### Error de Compilación

**Causa**: Compilador no soporta C++11

**Solución**:
```bash
g++ --version  # Verificar versión (necesita 4.8+)

# Compilar explícitamente
g++ -std=c++11 -o esort main.cpp esort.cpp external_sorter.cpp
```

### Archivo de salida vacío

**Causa**: No se generaron chunks

**Solución**:
- Verificar que hay datos de entrada
- Revisar permisos de escritura en directorio

### "El archivo NO está ordenado correctamente"

**Causa**: Bug en la lógica de fusión

**Solución**:
```bash
# Ver contenido del archivo
cat output.sorted.txt

# Verificar manualmente
sort -n output.sorted.txt > expected.txt
diff output.sorted.txt expected.txt
```

---

## 📝 Comandos Make Útiles

```bash
make              # Compilar todo
make clean        # Limpiar todo (ejecutables + temporales)
make clean-temp   # Limpiar solo archivos .tmp
make run          # Compilar y ejecutar
make verify       # Verificar ordenamiento
make show-output  # Mostrar archivo de salida
make help         # Ver todos los comandos
```

---

## 🎓 Conceptos Clave

### Fase 1: Adquisición
- Lee datos del origen (serial/archivo)
- Llena buffer hasta capacidad máxima
- Ordena buffer internamente
- Escribe chunk ordenado a disco
- Repite hasta procesar todos los datos

### Fase 2: Fusión (K-Way Merge)
- Abre K archivos chunk simultáneamente
- Lee primer elemento de cada uno
- Encuentra el mínimo entre los K valores
- Escribe mínimo a salida
- Avanza solo en el archivo usado
- Repite hasta agotar todos los archivos

---

## 💡 Tips

1. **Para pruebas rápidas**: Usa buffer pequeño (10) para ver múltiples chunks
2. **Para eficiencia**: Usa buffer grande (1000+) para menos I/O
3. **Para debugging**: Agrega prints en `external_sorter.cpp`
4. **Para Arduino**: Verifica baud rate coincide (9600)
5. **Para memoria**: Usa `valgrind ./esort` para detectar fugas

---

## 📚 Lectura Adicional

- `README.md` - Documentación completa del proyecto
- `DOCUMENTACION_TECNICA.md` - Análisis técnico profundo
- Código fuente - Comentado extensivamente

---

## ✅ Checklist de Entrega

- [ ] Compilación exitosa sin warnings
- [ ] Ejecuta `make verify` y pasa
- [ ] Probado con múltiples tamaños de buffer
- [ ] Sin fugas de memoria (valgrind)
- [ ] Código comentado
- [ ] README actualizado
- [ ] Archivos de prueba incluidos

---

**¿Dudas?** Revisa la documentación técnica o los comentarios en el código.

**Universidad Politécnica de Victoria - ITI**
