#ifndef ESORT_H
#define ESORT_H

#include <fstream>

// ============================================================================
// CLASE BASE ABSTRACTA: DataSource
// ============================================================================
class DataSource {
public:
    virtual int getNext() = 0;
    virtual bool hasMoreData() = 0;
    virtual ~DataSource() {}
};

// ============================================================================
// CLASE CONCRETA: SerialSource (Lee del Puerto Serial)
// ============================================================================
class SerialSource : public DataSource {
private:
    int fileDescriptor;  // Para Linux/POSIX
    bool connected;
    char buffer[256];
    int bufferPos;
    int bufferLen;
    
    bool readLineFromSerial(char* line, int maxLen);
    
public:
    SerialSource(const char* portName);
    ~SerialSource();
    
    int getNext() override;
    bool hasMoreData() override;
    void close();
};

// ============================================================================
// CLASE CONCRETA: FileSource (Lee de un archivo)
// ============================================================================
class FileSource : public DataSource {
private:
    std::ifstream file;
    bool isOpen;
    
public:
    FileSource(const char* filename);
    ~FileSource();
    
    int getNext() override;
    bool hasMoreData() override;
};

// ============================================================================
// NODO DE LA LISTA CIRCULAR
// ============================================================================
struct Node {
    int data;
    Node* next;
    Node* prev;
    
    Node(int value) : data(value), next(nullptr), prev(nullptr) {}
};

// ============================================================================
// CLASE: CircularBuffer (Lista Circular Doblemente Enlazada)
// ============================================================================
class CircularBuffer {
private:
    Node* head;
    int capacity;
    int currentSize;
    
public:
    CircularBuffer(int cap);
    ~CircularBuffer();
    
    bool insert(int value);
    bool isFull() const;
    int getSize() const;
    void sort();  // Ordenamiento interno (Insertion Sort)
    void writeToFile(const char* filename);
    void clear();
    
    // Para debugging
    void display() const;
};

// ============================================================================
// CLASE: ExternalSorter (Controlador Principal del Sistema)
// ============================================================================
class ExternalSorter {
private:
    CircularBuffer* buffer;
    int chunkCount;
    int bufferCapacity;
    
    void phase1_Acquisition(DataSource* source);
    void phase2_Merge(int numChunks, const char* outputFile);
    
    // Funciones auxiliares
    void generateChunkFilename(int chunkIndex, char* filename);
    int findMinimum(int* values, bool* active, int k);
    
public:
    ExternalSorter(int bufferSize);
    ~ExternalSorter();
    
    void run(DataSource* source, const char* outputFile);
};

// ============================================================================
// FUNCIONES AUXILIARES (Utilidades)
// ============================================================================
namespace Utils {
    int stringToInt(const char* str);
    void intToString(int value, char* buffer);
    int stringLength(const char* str);
    void stringCopy(char* dest, const char* src);
    void stringConcat(char* dest, const char* src);
}

#endif // ESORT_H
