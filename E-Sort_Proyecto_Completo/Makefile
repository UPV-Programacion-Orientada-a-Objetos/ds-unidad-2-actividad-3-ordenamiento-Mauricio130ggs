# Makefile para el proyecto E-Sort
# Sistema de Ordenamiento Externo

CXX = g++
CXXFLAGS = -Wall -std=c++11 -g
TARGET = esort
SIMULATOR = arduino_simulator

# Archivos objeto
OBJS = main.o esort.o external_sorter.o

# Regla principal
all: $(TARGET) $(SIMULATOR)

# Compilar el programa principal
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)
	@echo "==================================="
	@echo "Compilacion exitosa: $(TARGET)"
	@echo "==================================="

# Compilar el simulador de Arduino
$(SIMULATOR): arduino_simulator.cpp
	$(CXX) $(CXXFLAGS) -o $(SIMULATOR) arduino_simulator.cpp
	@echo "Simulador compilado: $(SIMULATOR)"

# Reglas de compilación de objetos
main.o: main.cpp esort.h
	$(CXX) $(CXXFLAGS) -c main.cpp

esort.o: esort.cpp esort.h
	$(CXX) $(CXXFLAGS) -c esort.cpp

external_sorter.o: external_sorter.cpp esort.h
	$(CXX) $(CXXFLAGS) -c external_sorter.cpp

# Ejecutar el programa con datos de prueba
run: $(TARGET)
	@echo "==================================="
	@echo "Ejecutando E-Sort..."
	@echo "==================================="
	./$(TARGET)

# Ejecutar el simulador de Arduino
run-simulator: $(SIMULATOR)
	@echo "==================================="
	@echo "Ejecutando simulador de Arduino..."
	@echo "==================================="
	./$(SIMULATOR)

# Limpiar archivos generados
clean:
	rm -f $(OBJS) $(TARGET) $(SIMULATOR)
	rm -f chunk_*.tmp
	rm -f output.sorted.txt
	rm -f test_input.txt
	@echo "Archivos limpiados."

# Limpiar solo archivos temporales (mantener ejecutables)
clean-temp:
	rm -f chunk_*.tmp
	rm -f test_input.txt
	@echo "Archivos temporales limpiados."

# Mostrar el contenido del archivo de salida
show-output:
	@if [ -f output.sorted.txt ]; then \
		echo "==================================="; \
		echo "Contenido de output.sorted.txt:"; \
		echo "==================================="; \
		cat output.sorted.txt; \
		echo ""; \
		echo "Total de lineas: $$(wc -l < output.sorted.txt)"; \
	else \
		echo "El archivo output.sorted.txt no existe."; \
	fi

# Verificar que la salida esté ordenada
verify:
	@if [ -f output.sorted.txt ]; then \
		echo "Verificando ordenamiento..."; \
		if sort -n output.sorted.txt | diff - output.sorted.txt > /dev/null; then \
			echo "✓ El archivo esta correctamente ordenado!"; \
		else \
			echo "✗ ERROR: El archivo NO esta ordenado correctamente."; \
		fi; \
	else \
		echo "El archivo output.sorted.txt no existe."; \
	fi

# Ayuda
help:
	@echo "==================================="
	@echo "Makefile - Proyecto E-Sort"
	@echo "==================================="
	@echo "Comandos disponibles:"
	@echo "  make              - Compilar todo"
	@echo "  make run          - Ejecutar E-Sort"
	@echo "  make run-simulator - Ejecutar simulador Arduino"
	@echo "  make clean        - Limpiar todo"
	@echo "  make clean-temp   - Limpiar archivos temporales"
	@echo "  make show-output  - Mostrar archivo de salida"
	@echo "  make verify       - Verificar ordenamiento"
	@echo "  make help         - Mostrar esta ayuda"
	@echo "==================================="

.PHONY: all run run-simulator clean clean-temp show-output verify help
