#ifndef DATATABLE_H
#define DATATABLE_H

#ifndef CPP_TESTING
#include <Arduino.h>
#define PRINT(msg)  Serial.println(F(msg));
#define MAX_VAR_SIZE 4
#else
#include <iostream>
using namespace std;
#define PRINT(msg)  cout << (msg) << endl;
#define MAX_VAR_SIZE 8
#endif
#include <stdarg.h>

// Currently supported datatypes
typedef enum {
    DataTable_undefinedType, // (not set)
    DataTable_UINT8,
    DataTable_ULONG,
    DataTable_INT,
    DataTable_UINT,
    DataTable_FLOAT,
} FieldType;

struct FieldFormat;

class DataTable
{
public:
    /**
     * @brief Create a new Table object
     * 
     * @param fieldCount fixed number of columns (fields per registry)
     * @param maxMem max total hardware memory to be used (in bytes)
     */
    DataTable(const uint8_t fieldCount, const unsigned int maxMem);
    ~DataTable();

    /**
     * @brief Set a new field
     * 
     * @param fieldIdx index of field (from 0 to fieldCount)
     * @param type data type of the field. Example: DataTable_INT
     */
    void setupField(uint8_t fieldIdx, FieldType type);

    /**
     * @brief Check table integrity, clear memory and order data
     * 
     * @param startMemPos Hardware pos to start writing the table
     */
    void begin(unsigned int startMemPos);

    /**
     * @brief Set Registry Values
     * 
     * @param index registry position in table
     * @param ... values
     */
    void setRegistry(int index, ...);

    /**
     * @brief Set primary key field index. Used to get empty registry positions
     * 
     * @param fieldId registry position in table
     * @param emptyValue pointer to empty value defined for Primary Key
     */
    void setPrimaryKey(uint8_t fieldId, void* emptyValue);

    /**
     * @brief Get a new empty pos according to Primary Key and it's defined empty value
     * 
     * @return int register position to use or (-1) if table is full
     */
    int newPos();

    /**
     * @brief Get Registry Values
     * 
     * @param index registry position in table
     * @param ... pointer to values
     */
    void getRegistry(int index, ...);


    /**
     * @brief Get lenght of table (in registries)
     * 
     * @return unsigned int = Max registries
     */
    inline unsigned int lenght() { return maxRegistries; };

    /**
     * @brief Find first occurrence of value in field
     * 
     * @param fieldIndex Field (column) to iterate
     * @param query pointer to value to look for
     * @return unsigned int = index of registry found (or -1 if not)
     */
    int findValue(uint8_t fieldIndex, void* query);

protected:
    // Fields (data)
    FieldFormat *fieldFormat = NULL;
    const uint8_t fieldCount; // (Columns)
    uint8_t primaryKey = 0;
    uint8_t pKEmptyValue[MAX_VAR_SIZE];

    // Registries (set of fields, single row)
    uint8_t registrySize = 0; // (Row size)
    unsigned int maxRegistries = 0; // Max rows

    // Memory data and address
    const unsigned int maxMemory;
    unsigned int startMemory = 0;
    unsigned int startTable = 0;

    // General functions
    void cleanReservedMemory(uint8_t byte = 0);

    // Wrappers for EEPROM, SPIFFS, etc
    virtual void beginHard(unsigned int hardMemSize) = 0;
    virtual void writeHard(unsigned int memPos, uint8_t data) = 0;
    virtual uint8_t readHard(unsigned int memPos) = 0;
};


#endif