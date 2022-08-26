#ifndef DATATABLE_H
#define DATATABLE_H

#include <Arduino.h>
#include <stdarg.h>

typedef enum {
    DataTable_undefinedType,
    DataTable_UINT8,
    DataTable_ULONG,
    DataTable_INT,
    DataTable_UINT,
    DataTable_FLOAT,
} FieldType;


struct FieldFormat
{
    FieldFormat() { size = 0; type = DataTable_undefinedType; }
    uint8_t size;
    uint8_t type;
};


class DataTable
{
public:
    DataTable(uint8_t fieldCount, unsigned int maxMem)
    {
        fieldFormat = new FieldFormat[fieldCount]();
        this->fieldCount = fieldCount;
        this->maxMemory = maxMem;
    }
    ~DataTable() 
    {
        delete[] fieldFormat;
    }

    void addField(uint8_t fieldIdx, uint8_t size, FieldType type)
    {
        if(fieldIdx < fieldCount)
        {
            fieldFormat[fieldIdx].type = type;
            fieldFormat[fieldIdx].size = size;
        }
    }

    /**
     * @brief Check table integrity, clear memory and order data
     * 
     * @param startMemPos 
     * @param totalHardMemory To init hardware
     */
    void begin(unsigned int startMemPos, unsigned int totalHardMemory)
    {
        // Check table initialized
        if(fieldCount == 0)
        {
            Serial.println(F("NULL DataTable"));
            _delay_ms(1000);
            return;
        }

        // Check field data ok
        for (uint8_t i = 0; i < fieldCount; i++)
        {
            if(fieldFormat[i].type == DataTable_undefinedType)
            {                
                Serial.println(F("DataTable ERR"));
                _delay_ms(1000);
                return;
            }
        }
        
        // TODO: Add field format memory checking (field bytes at header)
        // TODO: Add memory cleaning
    }

    /**
     * @brief Add new registry
     * 
     * @param ... 
     */
    void addRegistry(unsigned int& index, ...)
    {
        va_list args;
        va_start(args, index);

        for (uint8_t i = 0; i < fieldCount; i++)
        {
            uint8_t data[4];
            uint8_t dataSize;

            switch (fieldFormat[i].type)
            {
                case DataTable_UINT8:
                    data[0] = va_arg(args, uint8_t);
                    dataSize = sizeof(uint8_t);
                    break;
                case DataTable_ULONG:
                    *((unsigned long*)data) = va_arg(args, unsigned long);
                    dataSize = sizeof(unsigned long);
                    break;
                case DataTable_INT:
                    *((int*)data) = va_arg(args, int);
                    dataSize = sizeof(int);
                    break;
                case DataTable_UINT:
                    *((unsigned int*)data) = va_arg(args, unsigned int);
                    dataSize = sizeof(unsigned int);
                    break;
                case DataTable_FLOAT:
                    *((float*)data) = va_arg(args, float);
                    dataSize = sizeof(float);
                    break;
            
            default:
                break;
            }

            // TODO
        }
        

        va_end(args);
    }

private:
    FieldFormat *fieldFormat = NULL;
    uint8_t fieldCount = 0;

    unsigned int maxMemory = 0;

    // Wrappers for EEPROM, SPIFFS, etc
    void beginHard(unsigned int hardMemSize);
    void writeHard(unsigned int idx, uint8_t data);
    uint8_t readHard(unsigned int idx);
};

#endif