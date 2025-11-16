#include "esort.h"
#include <iostream>

using namespace std;

// ============================================================================
// IMPLEMENTACIÓN: ExternalSorter
// ============================================================================

ExternalSorter::ExternalSorter(int bufferSize) {
    bufferCapacity = bufferSize;
    buffer = new CircularBuffer(bufferSize);
    chunkCount = 0;
}

ExternalSorter::~ExternalSorter() {
    delete buffer;
}

void ExternalSorter::generateChunkFilename(int chunkIndex, char* filename) {
    Utils::stringCopy(filename, "chunk_");
    
    char indexStr[10];
    Utils::intToString(chunkIndex, indexStr);
    Utils::stringConcat(filename, indexStr);
    Utils::stringConcat(filename, ".tmp");
}

void ExternalSorter::phase1_Acquisition(DataSource* source) {
    cout << "\nIniciando Fase 1: Adquisicion de datos...\n" << endl;
    
    while (source->hasMoreData()) {
        int value = source->getNext();
        
        // Verificar si es un valor válido (algunos sistemas retornan 0 al finalizar)
        if (!source->hasMoreData() && value == 0) {
            break;
        }
        
        cout << "Leyendo -> " << value << endl;
        
        if (!buffer->insert(value)) {
            // Buffer lleno, procesar
            cout << "Buffer lleno. Ordenando internamente..." << endl;
            
            buffer->sort();
            
            cout << "Buffer ordenado: ";
            buffer->display();
            
            // Generar nombre del archivo chunk
            char filename[50];
            generateChunkFilename(chunkCount, filename);
            
            cout << "Escribiendo " << filename << "... ";
            buffer->writeToFile(filename);
            cout << "OK." << endl;
            
            buffer->clear();
            cout << "Buffer limpiado.\n" << endl;
            
            chunkCount++;
            
            // Insertar el valor actual en el buffer limpio
            buffer->insert(value);
        }
    }
    
    // Procesar los datos restantes en el buffer
    if (buffer->getSize() > 0) {
        cout << "Procesando datos restantes en buffer..." << endl;
        buffer->sort();
        
        cout << "Buffer ordenado: ";
        buffer->display();
        
        char filename[50];
        generateChunkFilename(chunkCount, filename);
        
        cout << "Escribiendo " << filename << "... ";
        buffer->writeToFile(filename);
        cout << "OK." << endl;
        
        buffer->clear();
        chunkCount++;
    }
    
    cout << "\nFase 1 completada. " << chunkCount << " chunks generados.\n" << endl;
}

int ExternalSorter::findMinimum(int* values, bool* active, int k) {
    int minIndex = -1;
    int minValue = 0;
    
    for (int i = 0; i < k; i++) {
        if (active[i]) {
            if (minIndex == -1 || values[i] < minValue) {
                minValue = values[i];
                minIndex = i;
            }
        }
    }
    
    return minIndex;
}

void ExternalSorter::phase2_Merge(int numChunks, const char* outputFile) {
    cout << "Iniciando Fase 2: Fusion Externa (K-Way Merge)" << endl;
    cout << "Abriendo " << numChunks << " archivos fuente..." << endl;
    
    // Crear arreglo de DataSource (FileSource)
    DataSource** sources = new DataSource*[numChunks];
    int* currentValues = new int[numChunks];
    bool* activeStreams = new bool[numChunks];
    bool* hasValidValue = new bool[numChunks];
    
    // Abrir todos los archivos chunk
    for (int i = 0; i < numChunks; i++) {
        char filename[50];
        generateChunkFilename(i, filename);
        
        sources[i] = new FileSource(filename);
        
        // Intentar leer el primer valor
        if (sources[i]->hasMoreData()) {
            currentValues[i] = sources[i]->getNext();
            hasValidValue[i] = true;
            activeStreams[i] = true;
        } else {
            hasValidValue[i] = false;
            activeStreams[i] = false;
        }
    }
    
    // Abrir archivo de salida
    ofstream outFile(outputFile);
    if (!outFile.is_open()) {
        cerr << "ERROR: No se pudo crear archivo de salida." << endl;
        return;
    }
    
    cout << "K=" << numChunks << ". Fusion en progreso..." << endl;
    
    int totalWritten = 0;
    
    // K-Way Merge
    while (true) {
        // Encontrar el mínimo entre todos los valores válidos
        int minIndex = -1;
        int minValue = 0;
        
        for (int i = 0; i < numChunks; i++) {
            if (hasValidValue[i]) {
                if (minIndex == -1 || currentValues[i] < minValue) {
                    minValue = currentValues[i];
                    minIndex = i;
                }
            }
        }
        
        if (minIndex == -1) {
            break;  // No hay más datos válidos
        }
        
        // Escribir el mínimo al archivo de salida
        outFile << minValue << "\n";
        
        // Mostrar progreso
        if (totalWritten < 20 || totalWritten % 10 == 0) {
            cout << " - Escribiendo " << minValue << " desde chunk_" << minIndex << ".tmp" << endl;
        }
        
        totalWritten++;
        
        // Intentar leer el siguiente valor del stream que usamos
        if (sources[minIndex]->hasMoreData()) {
            currentValues[minIndex] = sources[minIndex]->getNext();
            hasValidValue[minIndex] = true;
        } else {
            hasValidValue[minIndex] = false;
            activeStreams[minIndex] = false;
        }
    }
    
    outFile.close();
    
    cout << "\nFusion completada. Total de elementos escritos: " << totalWritten << endl;
    cout << "Archivo final: " << outputFile << endl;
    
    // Liberar memoria
    for (int i = 0; i < numChunks; i++) {
        delete sources[i];
    }
    delete[] sources;
    delete[] currentValues;
    delete[] activeStreams;
    delete[] hasValidValue;
}

void ExternalSorter::run(DataSource* source, const char* outputFile) {
    cout << "========================================" << endl;
    cout << "Sistema de Ordenamiento Externo E-Sort" << endl;
    cout << "========================================" << endl;
    cout << "Capacidad del buffer: " << bufferCapacity << " elementos" << endl;
    
    // Fase 1: Adquisición y segmentación
    phase1_Acquisition(source);
    
    // Fase 2: Fusión externa
    if (chunkCount > 0) {
        phase2_Merge(chunkCount, outputFile);
    } else {
        cout << "No se generaron chunks. No hay datos para procesar." << endl;
    }
    
    cout << "\nLiberando memoria... Sistema apagado." << endl;
}
