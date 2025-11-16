#include "esort.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;

int main() {
    // Configuración del sistema con un buffer pequeño para forzar múltiples chunks
    const int BUFFER_SIZE = 10;  // Buffer pequeño para crear múltiples chunks
    const char* OUTPUT_FILE = "output.sorted.txt";
    
    cout << "==========================================" << endl;
    cout << "PRUEBA EXTENDIDA - Sistema E-Sort" << endl;
    cout << "==========================================" << endl;
    cout << "Buffer size: " << BUFFER_SIZE << " elementos" << endl;
    cout << "Esto forzara la creacion de multiples chunks.\n" << endl;
    
    // Crear el ordenador externo
    ExternalSorter* sorter = new ExternalSorter(BUFFER_SIZE);
    
    // Generar archivo de prueba con 50 datos aleatorios
    ofstream testFile("test_input_large.txt");
    srand(time(0));
    
    cout << "Generando 50 numeros aleatorios para prueba..." << endl;
    for (int i = 0; i < 50; i++) {
        int value = rand() % 1000 + 1;
        testFile << value << "\n";
    }
    testFile.close();
    
    cout << "Archivo de prueba creado: test_input_large.txt\n" << endl;
    
    // Leer desde el archivo de prueba
    FileSource* fileSource = new FileSource("test_input_large.txt");
    
    if (fileSource->hasMoreData()) {
        sorter->run(fileSource, OUTPUT_FILE);
        
        // Verificar resultados
        cout << "\n==========================================" << endl;
        cout << "VERIFICACION DE RESULTADOS" << endl;
        cout << "==========================================" << endl;
        
        ifstream resultFile(OUTPUT_FILE);
        if (resultFile.is_open()) {
            int count = 0;
            int value, prev = -1;
            bool isOrdered = true;
            
            cout << "\nPrimeros 15 elementos: ";
            while (resultFile >> value) {
                if (count < 15) {
                    cout << value << " ";
                }
                
                // Verificar orden
                if (prev != -1 && value < prev) {
                    isOrdered = false;
                }
                prev = value;
                count++;
            }
            cout << "...\n" << endl;
            
            cout << "Total de elementos: " << count << endl;
            
            if (isOrdered) {
                cout << "✓ Todos los elementos estan CORRECTAMENTE ordenados!" << endl;
            } else {
                cout << "✗ ERROR: Los elementos NO estan ordenados correctamente." << endl;
            }
            
            resultFile.close();
        }
    } else {
        cout << "ERROR: No se pudo leer el archivo de prueba." << endl;
    }
    
    delete fileSource;
    delete sorter;
    
    cout << "\n==========================================" << endl;
    cout << "Prueba completada." << endl;
    cout << "==========================================" << endl;
    
    return 0;
}
