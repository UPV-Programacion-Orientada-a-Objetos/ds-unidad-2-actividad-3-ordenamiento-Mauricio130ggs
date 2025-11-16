# Sistema E-Sort - Ordenamiento Externo para Telemetría Masiva

## 📁 Estructura del Proyecto

```
E-Sort_Proyecto_Completo/
│
├── Makefile                          # Script de compilación
│
├── src/                              # 📂 Código Fuente Principal
│   ├── esort.h                       # Definiciones de clases
│   ├── esort.cpp                     # Implementación base
│   ├── external_sorter.cpp           # Algoritmo principal
│   └── main.cpp                      # Programa principal
│
├── docs/                             # 📖 Documentación
│   ├── README.md                     # Documentación completa
│   ├── DOCUMENTACION_TECNICA.md      # Análisis técnico profundo
│   ├── GUIA_RAPIDA.md                # Inicio rápido (5 min)
│   ├── RESUMEN_PROYECTO.md           # Resumen y checklist
│   └── ESTRUCTURA_PROYECTO.txt       # Estructura visual
│
├── tests/                            # 🧪 Programas de Prueba
│   ├── test_extended.cpp             # Prueba con múltiples chunks
│   ├── arduino_simulator.cpp         # Simulador de Arduino
│   └── arduino_telemetry.ino         # Código para Arduino real
│
└── scripts/                          # 🎬 Scripts Útiles
    └── demo.sh                       # Demostración visual interactiva
```

## 🚀 Inicio Rápido

### 1. Compilar el Proyecto

```bash
# Opción 1: Usando Make (recomendado)
make

# Opción 2: Manual
cd src
g++ -std=c++11 -g -c esort.cpp
g++ -std=c++11 -g -c external_sorter.cpp
g++ -std=c++11 -g -c main.cpp
g++ -std=c++11 -g -o ../esort main.o esort.o external_sorter.o
cd ..
```

### 2. Ejecutar

```bash
./esort
```

### 3. Verificar

```bash
make verify
```

## 📚 Documentación

- **Para empezar**: `docs/GUIA_RAPIDA.md`
- **Para uso general**: `docs/README.md`
- **Para análisis técnico**: `docs/DOCUMENTACION_TECNICA.md`
- **Para entrega**: `docs/RESUMEN_PROYECTO.md`

## 🎯 Características

✅ Ordenamiento Externo (External Mergesort)  
✅ K-Way Merge funcional  
✅ Lista Circular Doblemente Enlazada  
✅ Sin STL (implementación manual)  
✅ POO: Herencia, polimorfismo, clases abstractas  
✅ Soporte para puerto serial (Arduino)  
✅ Documentación profesional completa  

## 📋 Comandos Make

```bash
make              # Compilar todo
make run          # Compilar y ejecutar
make verify       # Verificar ordenamiento
make clean        # Limpiar archivos generados
make help         # Ver todos los comandos
```

## 🧪 Pruebas

```bash
# Prueba básica (30 elementos)
./esort

# Prueba extendida (50 elementos, múltiples chunks)
cd src
g++ -o ../test_extended test_extended.cpp esort.o external_sorter.o
cd ..
./test_extended

# Demostración visual
chmod +x scripts/demo.sh
./scripts/demo.sh
```

## 📦 Archivos Generados

Al ejecutar, se crearán:
- `chunk_0.tmp`, `chunk_1.tmp`, ... (archivos temporales ordenados)
- `output.sorted.txt` (resultado final ordenado)

## 🎓 Proyecto Académico

**Universidad**: Politécnica de Victoria  
**Carrera**: ITI - Ingeniería en Tecnologías de la Información  
**Materia**: Estructuras de Datos  
**Tipo**: Caso de Estudio - Ordenamiento Externo

---

✨ **Proyecto completado y verificado** ✨
