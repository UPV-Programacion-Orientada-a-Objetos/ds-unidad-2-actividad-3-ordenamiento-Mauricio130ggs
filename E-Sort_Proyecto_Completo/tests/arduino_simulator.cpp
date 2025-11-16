/*
 * SIMULADOR DE ARDUINO - Generador de Datos de Telemetría
 * 
 * Este programa simula un Arduino enviando datos por puerto serial.
 * Puede usarse para probar el sistema E-Sort sin necesidad de hardware real.
 * 
 * Compilar: g++ -o arduino_simulator arduino_simulator.cpp
 * Ejecutar: ./arduino_simulator > /dev/ttyUSB0  (Linux)
 *           ./arduino_simulator > COM3           (Windows con redirección)
 */

#include <iostream>
#include <cstdlib>
#include <ctime>

#ifdef _WIN32
    #include <windows.h>
    #define SLEEP(ms) Sleep(ms)
#else
    #include <unistd.h>
    #define SLEEP(ms) usleep((ms) * 1000)
#endif

using namespace std;

int main() {
    srand(time(0));
    
    cerr << "Arduino Simulator - Generador de Telemetria" << endl;
    cerr << "Generando datos cada 100ms..." << endl;
    cerr << "Presiona Ctrl+C para detener" << endl;
    cerr << "-------------------------------------------" << endl;
    
    // Generar 100 valores de telemetría
    for (int i = 0; i < 100; i++) {
        // Generar un valor aleatorio entre 1 y 1000
        int value = rand() % 1000 + 1;
        
        // Enviar por stdout (que puede ser redirigido al puerto serial)
        cout << value << endl;
        cout.flush();
        
        // Log en stderr para no interferir con los datos
        cerr << "[" << i + 1 << "] Enviado: " << value << endl;
        
        // Simular delay de lectura del sensor
        SLEEP(100);  // 100ms entre lecturas
    }
    
    cerr << "-------------------------------------------" << endl;
    cerr << "Transmision completada. 100 valores enviados." << endl;
    
    return 0;
}
