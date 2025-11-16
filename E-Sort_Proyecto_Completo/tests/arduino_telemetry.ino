/*
 * GENERADOR DE TELEMETRIA - ARDUINO
 * 
 * Este sketch simula un detector de física de partículas
 * que genera lecturas de energía (valores enteros).
 * 
 * Conexión:
 * - Conectar Arduino via USB al ordenador
 * - El programa en C++ leerá del puerto serial correspondiente
 *   (/dev/ttyUSB0, /dev/ttyACM0, COM3, etc.)
 * 
 * Configuración:
 * - Velocidad: 9600 baud
 * - Formato: 8N1 (8 bits, sin paridad, 1 stop bit)
 */

// Pin del sensor analógico (puede ser un potenciómetro o sensor real)
const int SENSOR_PIN = A0;

// Configuración
const int BAUD_RATE = 9600;
const int DELAY_MS = 100;  // Delay entre lecturas (100ms = 10 lecturas/segundo)

// Variables para generación de datos
int readingCount = 0;
const int MAX_READINGS = 500;  // Generar 500 lecturas y detenerse

void setup() {
  // Inicializar comunicación serial
  Serial.begin(BAUD_RATE);
  
  // Inicializar generador aleatorio con ruido del pin analógico
  randomSeed(analogRead(A1));
  
  // Dar tiempo para establecer conexión
  delay(2000);
  
  Serial.println("# Arduino - Generador de Telemetria");
  Serial.println("# Detector de Particulas Simulado");
  Serial.println("# ===================================");
  delay(500);
}

void loop() {
  if (readingCount < MAX_READINGS) {
    // OPCIÓN 1: Leer valor de un sensor real (descomenta para usar)
    // int sensorValue = analogRead(SENSOR_PIN);
    // int energyReading = map(sensorValue, 0, 1023, 1, 1000);
    
    // OPCIÓN 2: Generar valores simulados (por defecto)
    int energyReading = generateSimulatedReading();
    
    // Enviar el valor por serial
    Serial.println(energyReading);
    
    // Incrementar contador
    readingCount++;
    
    // Delay entre lecturas
    delay(DELAY_MS);
    
  } else {
    // Después de enviar todas las lecturas, detener
    Serial.println("# Transmision completada");
    
    // Entrar en loop infinito (puedes presionar Reset para reiniciar)
    while (true) {
      delay(1000);
    }
  }
}

/**
 * Genera un valor simulado de energía
 * Simula diferentes niveles de energía detectada
 */
int generateSimulatedReading() {
  // Generar patrones realistas de detección
  int patternType = random(0, 100);
  
  if (patternType < 60) {
    // 60% - Lecturas de fondo (background radiation)
    return random(1, 100);
    
  } else if (patternType < 85) {
    // 25% - Eventos de energía media
    return random(100, 500);
    
  } else if (patternType < 95) {
    // 10% - Eventos de alta energía
    return random(500, 800);
    
  } else {
    // 5% - Eventos excepcionales de muy alta energía
    return random(800, 1000);
  }
}

/**
 * MODO ALTERNATIVO: Pulsos periódicos
 * Descomenta esta función y úsala en lugar de generateSimulatedReading()
 */
/*
int generatePeriodicPulse() {
  // Generar pulsos sinusoidales
  float angle = (readingCount * 0.1);
  int baseValue = 500;
  int amplitude = 400;
  int noise = random(-50, 50);
  
  int value = baseValue + (amplitude * sin(angle)) + noise;
  
  // Asegurar que el valor esté en rango válido
  if (value < 1) value = 1;
  if (value > 1000) value = 1000;
  
  return value;
}
*/

/**
 * MODO ALTERNATIVO: Eventos de Poisson
 * Simula eventos raros siguiendo distribución de Poisson
 */
/*
int generatePoissonEvents() {
  int baseReading = random(1, 50);
  
  // Evento raro (1% de probabilidad)
  if (random(0, 100) < 1) {
    baseReading += random(500, 1000);
  }
  
  return baseReading;
}
*/
