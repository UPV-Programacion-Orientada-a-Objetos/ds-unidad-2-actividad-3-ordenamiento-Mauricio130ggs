#!/bin/bash

# Script de Demostración Visual - E-Sort
# Muestra el funcionamiento del sistema paso a paso

echo "╔════════════════════════════════════════════════════════════╗"
echo "║       DEMOSTRACIÓN VISUAL - Sistema E-Sort                 ║"
echo "║       Ordenamiento Externo para Telemetría Masiva          ║"
echo "╚════════════════════════════════════════════════════════════╝"
echo ""

# Colores
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Función para pausar
pause() {
    echo -e "\n${YELLOW}Presiona Enter para continuar...${NC}"
    read
}

echo -e "${CYAN}📋 PASO 1: Verificando archivos del proyecto${NC}"
echo "─────────────────────────────────────────────────"
if [ -f "esort.h" ] && [ -f "esort.cpp" ] && [ -f "main.cpp" ]; then
    echo -e "${GREEN}✓ Todos los archivos principales encontrados${NC}"
    ls -lh *.h *.cpp Makefile 2>/dev/null | awk '{print "  " $9 " (" $5 ")"}'
else
    echo -e "${RED}✗ Faltan archivos del proyecto${NC}"
    exit 1
fi
pause

echo -e "\n${CYAN}🔨 PASO 2: Limpiando compilaciones anteriores${NC}"
echo "─────────────────────────────────────────────────"
make clean 2>&1 | grep -v "make:"
echo -e "${GREEN}✓ Archivos temporales eliminados${NC}"
pause

echo -e "\n${CYAN}⚙️  PASO 3: Compilando el proyecto${NC}"
echo "─────────────────────────────────────────────────"
make 2>&1 | grep -E "(Compilacion|esort)"
if [ -f "esort" ]; then
    echo -e "${GREEN}✓ Compilación exitosa${NC}"
else
    echo -e "${RED}✗ Error en compilación${NC}"
    exit 1
fi
pause

echo -e "\n${CYAN}🎲 PASO 4: Generando datos de prueba${NC}"
echo "─────────────────────────────────────────────────"
# Generar archivo de prueba personalizado
cat > test_demo.txt << EOF
850
42
999
15
567
88
234
777
111
456
333
622
199
888
25
EOF

echo "Datos generados (15 valores desordenados):"
cat test_demo.txt | tr '\n' ' '
echo -e "\n${GREEN}✓ Archivo test_demo.txt creado${NC}"
pause

echo -e "\n${CYAN}📊 PASO 5: Visualizando los datos de entrada${NC}"
echo "─────────────────────────────────────────────────"
echo "Valores en orden de llegada:"
cat test_demo.txt | awk '{printf "%4d ", $1; if (NR % 5 == 0) print ""}'
echo ""

echo -e "\nOrden esperado después del procesamiento:"
sort -n test_demo.txt | awk '{printf "%4d ", $1; if (NR % 5 == 0) print ""}'
echo ""
pause

echo -e "\n${CYAN}🚀 PASO 6: Ejecutando E-Sort${NC}"
echo "─────────────────────────────────────────────────"
echo -e "${YELLOW}Nota: El programa procesará los datos con buffer de 1000 elementos${NC}"
echo ""

# Modificar main.cpp temporalmente para usar test_demo.txt
cat > temp_main.cpp << 'EOFMAIN'
#include "esort.h"
#include <iostream>
using namespace std;

int main() {
    const int BUFFER_SIZE = 5;  // Buffer pequeño para demo
    const char* OUTPUT_FILE = "output.sorted.txt";
    
    cout << "Ejecutando E-Sort en modo demostración...\n" << endl;
    
    ExternalSorter* sorter = new ExternalSorter(BUFFER_SIZE);
    FileSource* fileSource = new FileSource("test_demo.txt");
    
    if (fileSource->hasMoreData()) {
        sorter->run(fileSource, OUTPUT_FILE);
    }
    
    delete fileSource;
    delete sorter;
    
    return 0;
}
EOFMAIN

# Compilar versión demo
g++ -std=c++11 -g -o esort_demo temp_main.cpp esort.o external_sorter.o 2>/dev/null

./esort_demo

rm temp_main.cpp esort_demo
pause

echo -e "\n${CYAN}📁 PASO 7: Examinando archivos generados${NC}"
echo "─────────────────────────────────────────────────"
echo "Archivos chunk creados:"
ls -lh chunk_*.tmp 2>/dev/null | awk '{print "  " $9 " - " $5}'

echo -e "\n${BLUE}Contenido de cada chunk:${NC}"
for chunk in chunk_*.tmp; do
    if [ -f "$chunk" ]; then
        echo -e "\n${YELLOW}$chunk:${NC}"
        cat "$chunk" | tr '\n' ' '
        echo ""
    fi
done
pause

echo -e "\n${CYAN}✅ PASO 8: Verificando resultado final${NC}"
echo "─────────────────────────────────────────────────"
if [ -f "output.sorted.txt" ]; then
    echo "Resultado ordenado (output.sorted.txt):"
    cat output.sorted.txt | awk '{printf "%4d ", $1; if (NR % 5 == 0) print ""}'
    echo -e "\n"
    
    # Verificar ordenamiento
    if sort -n output.sorted.txt | diff - output.sorted.txt > /dev/null 2>&1; then
        echo -e "${GREEN}✓✓✓ ¡El archivo está CORRECTAMENTE ordenado! ✓✓✓${NC}"
    else
        echo -e "${RED}✗ Error: El archivo NO está ordenado${NC}"
    fi
    
    # Estadísticas
    total=$(wc -l < output.sorted.txt)
    min=$(head -1 output.sorted.txt)
    max=$(tail -1 output.sorted.txt)
    
    echo -e "\n${BLUE}Estadísticas:${NC}"
    echo "  Total de elementos: $total"
    echo "  Valor mínimo: $min"
    echo "  Valor máximo: $max"
else
    echo -e "${RED}✗ No se generó el archivo de salida${NC}"
fi
pause

echo -e "\n${CYAN}🔍 PASO 9: Comparación Visual${NC}"
echo "─────────────────────────────────────────────────"
echo -e "${YELLOW}ANTES (desordenado):${NC}"
cat test_demo.txt | awk '{printf "%4d ", $1; if (NR % 5 == 0) print ""}'
echo -e "\n"

echo -e "${GREEN}DESPUÉS (ordenado):${NC}"
cat output.sorted.txt | awk '{printf "%4d ", $1; if (NR % 5 == 0) print ""}'
echo -e "\n"

echo -e "${BLUE}Diferencia visual:${NC}"
echo "  Primero → $(head -1 output.sorted.txt) (antes era $(head -1 test_demo.txt))"
echo "  Último  → $(tail -1 output.sorted.txt) (antes era $(tail -1 test_demo.txt))"
pause

echo -e "\n${CYAN}📈 PASO 10: Análisis de Rendimiento${NC}"
echo "─────────────────────────────────────────────────"
num_chunks=$(ls chunk_*.tmp 2>/dev/null | wc -l)
echo "  Chunks generados: $num_chunks"
echo "  Elementos por chunk (aprox): $(($total / $num_chunks))"
echo "  Operaciones de merge: $(($num_chunks * $total))"
echo "  Eficiencia: O(N·K) donde N=$total, K=$num_chunks"
pause

echo -e "\n${CYAN}🧹 PASO 11: Limpieza (opcional)${NC}"
echo "─────────────────────────────────────────────────"
echo "¿Deseas eliminar archivos temporales? (s/n)"
read -r respuesta
if [ "$respuesta" = "s" ] || [ "$respuesta" = "S" ]; then
    rm -f chunk_*.tmp test_demo.txt
    echo -e "${GREEN}✓ Archivos temporales eliminados${NC}"
else
    echo -e "${YELLOW}Archivos conservados para inspección${NC}"
fi

echo ""
echo "╔════════════════════════════════════════════════════════════╗"
echo "║                  DEMOSTRACIÓN COMPLETADA                   ║"
echo "╚════════════════════════════════════════════════════════════╝"
echo ""
echo -e "${GREEN}Resumen:${NC}"
echo "  ✓ Proyecto compilado correctamente"
echo "  ✓ Datos procesados y ordenados"
echo "  ✓ Algoritmo K-Way Merge ejecutado"
echo "  ✓ Resultado verificado"
echo ""
echo -e "${CYAN}Archivos generados:${NC}"
echo "  - output.sorted.txt (resultado final)"
if [ -f "chunk_0.tmp" ]; then
    echo "  - chunk_*.tmp (archivos temporales)"
fi
echo ""
echo -e "${YELLOW}Para más pruebas:${NC}"
echo "  ./esort              - Ejecutar con 30 elementos"
echo "  ./test_extended      - Ejecutar con 50 elementos (múltiples chunks)"
echo "  make verify          - Verificar ordenamiento"
echo ""
echo "¡Gracias por usar E-Sort!"
echo ""
