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

#include <stdarg.h> // varargs

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
    virtual ~DataTable();

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
    virtual void begin(unsigned int startMemPos);

    /**
     * @brief Begin table and set primary key field index and empty value, which is used to get empty registry positions
     * 
     * @param startMemPos Hardware pos to start writing the table
     * @param fieldId registry position in table
     * @param emptyValue empty value defined for Primary Key
     */
    virtual void beginPK(unsigned int startMemPos, uint8_t pkFieldId, ...);

    /**
     * @brief Set Registry Values
     * 
     * @param index registry position in table
     * @param ... values
     */
    void setRegistry(int index, ...);

    /**
     * @brief Delete registry by primary key
     * 
     * @param index Reg index in table
     */
    void delRegistry(int index);

    /**
     * @brief Check if registry is available (empty primary key)
     * 
     * @param index registry index
     * @retval true = available
     * @retval false = used
     */
    bool available(int index);

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
     * @brief Erase all table primary keys according to defined empty values
     * 
     */
    void clean();

    /**
     * @brief Find first occurrence of value in field
     * 
     * @param fieldIndex Field (column) to iterate
     * @param query value to look for
     * @return unsigned int = index of registry found (or -1 if not)
     */
    int findValue(uint8_t fieldIndex, ...);

protected:
    // Fields (data)
    FieldFormat *fieldFormat = NULL;
    const uint8_t fieldCount; // (Columns)
    uint8_t primaryKey = 0;
    uint8_t pKEmptyValue[MAX_VAR_SIZE]; // empty value is 0x0 by default

    int findValueReferenced(uint8_t fieldIndex, void* data); // basic function to find a value

    // Registries (set of fields, single row)
    uint8_t registrySize = 0; // (Row size)
    unsigned int maxRegistries = 0; // Max rows

    // Memory data and address
    const unsigned int maxMemory;
    unsigned int startMemory = 0;
    unsigned int startTable = 0;

    // General functions
    void cleanReservedMemory();

    // Wrappers for EEPROM, SPIFFS, etc
    virtual void beginHard(unsigned int hardMemSize) = 0; // FIXME: NOT USED
    virtual void writeHard(unsigned int memPos, uint8_t data) = 0;
    virtual uint8_t readHard(unsigned int memPos) = 0;
};


#endif