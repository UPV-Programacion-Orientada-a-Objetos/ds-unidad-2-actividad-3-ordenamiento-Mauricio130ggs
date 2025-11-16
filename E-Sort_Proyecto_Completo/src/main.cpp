#include "esort.h"
#include <iostream>

using namespace std;

int main() {
    // Configuración del sistema
    const int BUFFER_SIZE = 1000;  // Tamaño del buffer circular
    const char* OUTPUT_FILE = "output.sorted.txt";
    
    cout << "Iniciando Sistema de Ordenamiento Externo E-Sort..." << endl;
    
    // Crear el ordenador externo
    ExternalSorter* sorter = new ExternalSorter(BUFFER_SIZE);
    
    // Opción 1: Leer desde puerto serial (descomentar para usar Arduino real)
    /*
    const char* SERIAL_PORT = "/dev/ttyUSB0";  // Linux
    // const char* SERIAL_PORT = "COM3";       // Windows
    
    SerialSource* serialSource = new SerialSource(SERIAL_PORT);
    
    if (serialSource->hasMoreData()) {
        sorter->run(serialSource, OUTPUT_FILE);
    } else {
        cout << "No se pudo conectar al puerto serial." << endl;
    }
    
    delete serialSource;
    */
    
    // Opción 2: Usar archivo de prueba simulado (para testing)
    cout << "\nModo de prueba: Generando datos de prueba..." << endl;
    
    // Crear un archivo de prueba con datos desordenados
    ofstream testFile("test_input.txt");
    int testData[] = {105, 5, 210, 99, 1, 500, 20, 15, 88, 234, 12, 456, 
                      78, 901, 23, 567, 34, 123, 89, 345, 67, 789, 45, 
                      678, 56, 890, 321, 654, 987, 432};
    
    cout << "Datos de prueba generados: ";
    for (int i = 0; i < 30; i++) {
        testFile << testData[i] << "\n";
        cout << testData[i];
        if (i < 29) cout << ", ";
    }
    cout << endl;
    testFile.close();
    
    // Leer desde el archivo de prueba
    FileSource* fileSource = new FileSource("test_input.txt");
    
    if (fileSource->hasMoreData()) {
        sorter->run(fileSource, OUTPUT_FILE);
        
        // Mostrar los primeros resultados
        cout << "\n========================================" << endl;
        cout << "Verificando archivo de salida..." << endl;
        cout << "========================================" << endl;
        
        ifstream resultFile(OUTPUT_FILE);
        if (resultFile.is_open()) {
            cout << "Primeros 10 elementos ordenados:" << endl;
            int value;
            int count = 0;
            while (resultFile >> value && count < 10) {
                cout << value << " ";
                count++;
            }
            cout << "..." << endl;
            resultFile.close();
        }
    } else {
        cout << "No se pudo leer el archivo de prueba." << endl;
    }
    
    delete fileSource;
    delete sorter;
    
    cout << "\n========================================" << endl;
    cout << "Programa finalizado exitosamente." << endl;
    cout << "========================================" << endl;
    
    return 0;
}
