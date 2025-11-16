#include "esort.h"
#include <iostream>
#include <fstream>

// Para comunicación serial en Linux
#ifdef __linux__
    #include <fcntl.h>
    #include <unistd.h>
    #include <termios.h>
#elif _WIN32
    #include <windows.h>
#endif

using namespace std;

// ============================================================================
// IMPLEMENTACIÓN: Funciones Auxiliares (Utils)
// ============================================================================
namespace Utils {
    int stringToInt(const char* str) {
        int result = 0;
        int sign = 1;
        int i = 0;
        
        // Manejar signo negativo
        if (str[0] == '-') {
            sign = -1;
            i = 1;
        }
        
        while (str[i] != '\0' && str[i] >= '0' && str[i] <= '9') {
            result = result * 10 + (str[i] - '0');
            i++;
        }
        
        return result * sign;
    }
    
    void intToString(int value, char* buffer) {
        if (value == 0) {
            buffer[0] = '0';
            buffer[1] = '\0';
            return;
        }
        
        int isNegative = 0;
        if (value < 0) {
            isNegative = 1;
            value = -value;
        }
        
        char temp[20];
        int i = 0;
        
        while (value > 0) {
            temp[i++] = '0' + (value % 10);
            value /= 10;
        }
        
        int j = 0;
        if (isNegative) {
            buffer[j++] = '-';
        }
        
        while (i > 0) {
            buffer[j++] = temp[--i];
        }
        buffer[j] = '\0';
    }
    
    int stringLength(const char* str) {
        int len = 0;
        while (str[len] != '\0') len++;
        return len;
    }
    
    void stringCopy(char* dest, const char* src) {
        int i = 0;
        while (src[i] != '\0') {
            dest[i] = src[i];
            i++;
        }
        dest[i] = '\0';
    }
    
    void stringConcat(char* dest, const char* src) {
        int destLen = stringLength(dest);
        int i = 0;
        while (src[i] != '\0') {
            dest[destLen + i] = src[i];
            i++;
        }
        dest[destLen + i] = '\0';
    }
}

// ============================================================================
// IMPLEMENTACIÓN: SerialSource (Comunicación Serial)
// ============================================================================
#ifdef __linux__
SerialSource::SerialSource(const char* portName) {
    connected = false;
    bufferPos = 0;
    bufferLen = 0;
    
    cout << "Conectando a puerto " << portName << "... ";
    
    fileDescriptor = open(portName, O_RDWR | O_NOCTTY);
    
    if (fileDescriptor < 0) {
        cout << "ERROR al abrir puerto." << endl;
        return;
    }
    
    // Configurar el puerto serial
    struct termios tty;
    if (tcgetattr(fileDescriptor, &tty) != 0) {
        cout << "ERROR al obtener atributos." << endl;
        ::close(fileDescriptor);
        return;
    }
    
    // Configurar velocidad (9600 baud)
    cfsetospeed(&tty, B9600);
    cfsetispeed(&tty, B9600);
    
    // Configuración 8N1
    tty.c_cflag &= ~PARENB;  // Sin paridad
    tty.c_cflag &= ~CSTOPB;  // 1 stop bit
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;      // 8 bits
    tty.c_cflag &= ~CRTSCTS; // Sin control de flujo
    tty.c_cflag |= CREAD | CLOCAL;
    
    tty.c_lflag &= ~ICANON;  // Modo no canónico
    tty.c_lflag &= ~ECHO;
    tty.c_lflag &= ~ECHOE;
    tty.c_lflag &= ~ECHONL;
    tty.c_lflag &= ~ISIG;
    
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);
    
    tty.c_oflag &= ~OPOST;
    tty.c_oflag &= ~ONLCR;
    
    tty.c_cc[VTIME] = 10;
    tty.c_cc[VMIN] = 0;
    
    if (tcsetattr(fileDescriptor, TCSANOW, &tty) != 0) {
        cout << "ERROR al configurar puerto." << endl;
        ::close(fileDescriptor);
        return;
    }
    
    connected = true;
    cout << "Conectado." << endl;
}

SerialSource::~SerialSource() {
    close();
}

void SerialSource::close() {
    if (connected && fileDescriptor >= 0) {
        ::close(fileDescriptor);
        connected = false;
    }
}

bool SerialSource::readLineFromSerial(char* line, int maxLen) {
    int pos = 0;
    
    while (pos < maxLen - 1) {
        // Si el buffer interno está vacío, leer más datos
        if (bufferPos >= bufferLen) {
            bufferLen = read(fileDescriptor, buffer, sizeof(buffer) - 1);
            bufferPos = 0;
            
            if (bufferLen <= 0) {
                return false;  // No hay más datos
            }
        }
        
        char c = buffer[bufferPos++];
        
        if (c == '\n' || c == '\r') {
            if (pos > 0) {  // Solo retornar si tenemos algo
                line[pos] = '\0';
                return true;
            }
        } else {
            line[pos++] = c;
        }
    }
    
    line[pos] = '\0';
    return pos > 0;
}

int SerialSource::getNext() {
    char line[256];
    if (readLineFromSerial(line, 256)) {
        return Utils::stringToInt(line);
    }
    return 0;
}

bool SerialSource::hasMoreData() {
    return connected;
}

#elif _WIN32
// Implementación para Windows
SerialSource::SerialSource(const char* portName) {
    connected = false;
    bufferPos = 0;
    bufferLen = 0;
    
    cout << "Conectando a puerto " << portName << "... ";
    cout << "Conectado (simulado en Windows)." << endl;
    connected = true;
}

SerialSource::~SerialSource() {
    close();
}

void SerialSource::close() {
    connected = false;
}

bool SerialSource::readLineFromSerial(char* line, int maxLen) {
    // Simulación para Windows (retorna false para terminar)
    return false;
}

int SerialSource::getNext() {
    return 0;
}

bool SerialSource::hasMoreData() {
    return false;
}
#endif

// ============================================================================
// IMPLEMENTACIÓN: FileSource
// ============================================================================
FileSource::FileSource(const char* filename) {
    file.open(filename);
    isOpen = file.is_open();
    
    if (!isOpen) {
        cerr << "ERROR: No se pudo abrir " << filename << endl;
    }
}

FileSource::~FileSource() {
    if (isOpen) {
        file.close();
    }
}

int FileSource::getNext() {
    int value = 0;
    if (file >> value) {
        return value;
    }
    return 0;
}

bool FileSource::hasMoreData() {
    if (!isOpen) return false;
    
    // Verificar si hay más caracteres disponibles
    file >> ws;  // Saltar espacios en blanco
    return file.good() && file.peek() != EOF;
}

// ============================================================================
// IMPLEMENTACIÓN: CircularBuffer
// ============================================================================
CircularBuffer::CircularBuffer(int cap) {
    capacity = cap;
    currentSize = 0;
    head = nullptr;
}

CircularBuffer::~CircularBuffer() {
    clear();
}

bool CircularBuffer::insert(int value) {
    if (currentSize >= capacity) {
        return false;  // Buffer lleno
    }
    
    Node* newNode = new Node(value);
    
    if (head == nullptr) {
        // Primer nodo
        head = newNode;
        head->next = head;
        head->prev = head;
    } else {
        // Insertar al final (antes de head)
        Node* tail = head->prev;
        
        tail->next = newNode;
        newNode->prev = tail;
        newNode->next = head;
        head->prev = newNode;
    }
    
    currentSize++;
    return true;
}

bool CircularBuffer::isFull() const {
    return currentSize >= capacity;
}

int CircularBuffer::getSize() const {
    return currentSize;
}

void CircularBuffer::sort() {
    if (currentSize <= 1) return;
    
    // Insertion Sort sobre la lista circular
    Node* sorted = nullptr;
    Node* current = head;
    
    for (int i = 0; i < currentSize; i++) {
        Node* next = current->next;
        
        if (sorted == nullptr) {
            sorted = current;
            sorted->next = sorted;
            sorted->prev = sorted;
        } else {
            // Encontrar posición de inserción
            Node* pos = sorted;
            bool inserted = false;
            
            do {
                if (current->data < pos->data) {
                    // Insertar antes de pos
                    Node* prevNode = pos->prev;
                    
                    current->next = pos;
                    current->prev = prevNode;
                    prevNode->next = current;
                    pos->prev = current;
                    
                    if (pos == sorted) {
                        sorted = current;
                    }
                    
                    inserted = true;
                    break;
                }
                pos = pos->next;
            } while (pos != sorted && !inserted);
            
            if (!inserted) {
                // Insertar al final
                Node* tail = sorted->prev;
                current->next = sorted;
                current->prev = tail;
                tail->next = current;
                sorted->prev = current;
            }
        }
        
        current = next;
    }
    
    head = sorted;
}

void CircularBuffer::writeToFile(const char* filename) {
    ofstream outFile(filename);
    
    if (!outFile.is_open()) {
        cerr << "ERROR: No se pudo crear " << filename << endl;
        return;
    }
    
    if (head == nullptr) return;
    
    Node* current = head;
    for (int i = 0; i < currentSize; i++) {
        outFile << current->data << "\n";
        current = current->next;
    }
    
    outFile.close();
}

void CircularBuffer::clear() {
    if (head == nullptr) return;
    
    Node* current = head;
    for (int i = 0; i < currentSize; i++) {
        Node* next = current->next;
        delete current;
        current = next;
    }
    
    head = nullptr;
    currentSize = 0;
}

void CircularBuffer::display() const {
    if (head == nullptr) {
        cout << "[]" << endl;
        return;
    }
    
    cout << "[";
    Node* current = head;
    for (int i = 0; i < currentSize; i++) {
        cout << current->data;
        if (i < currentSize - 1) cout << ", ";
        current = current->next;
    }
    cout << "]" << endl;
}
