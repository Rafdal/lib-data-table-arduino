#include "DataTable.h"

// Function documentation is in the header file

#include <string.h>

struct FieldFormat
{
    FieldFormat()
    {
        type = DataTable_undefinedType; 
    }
    uint8_t type;
    uint8_t size()
    {
        switch (type)
        {
            case DataTable_UINT8:
                return sizeof(uint8_t);
                break;
            case DataTable_ULONG:
                return sizeof(unsigned long);
                break;
            case DataTable_INT:
                return sizeof(int);
                break;
            case DataTable_UINT:
                return sizeof(unsigned int);
                break;
            case DataTable_FLOAT:
                return sizeof(float);
                break;
        default:
            return 0;
            break;
        }
    }
};

DataTable::DataTable(const uint8_t fieldCount, const unsigned int maxMem) : fieldCount(fieldCount), maxMemory(maxMem)
{
    fieldFormat = new FieldFormat[fieldCount]();
    memset(pKEmptyValue, 0, MAX_VAR_SIZE); // empty value is 0x0 by default
}

DataTable::~DataTable() 
{
    delete[] fieldFormat;
}


void DataTable::setupField(uint8_t fieldIdx, FieldType type)
{
    if(fieldIdx < fieldCount && fieldFormat[fieldIdx].type == DataTable_undefinedType)
    {
        fieldFormat[fieldIdx].type = type;
        registrySize += fieldFormat[fieldIdx].size();
    }
    else
        PRINT("setField ERR");    
}


void DataTable::begin(unsigned int startMemPos)
{
    // Check table initialized
    if(fieldCount == 0 || registrySize == 0 || maxRegistries != 0)
    {
        PRINT("BAD DataTable");
        return;
    }

    startMemory = startMemPos;
    startTable = startMemPos + fieldCount;

    // Check field format memory ok
    bool doCleanFlag = false;
    for (uint8_t i = 0; i < fieldCount && !doCleanFlag; i++)
    {
        if(fieldFormat[i].type == DataTable_undefinedType)
        {                
            PRINT("DataTable ERR");
            return;
        }

        if(fieldFormat[i].type != readHard(startMemory + i))
        {
            doCleanFlag = true;
        }
    }

    maxRegistries = (maxMemory - fieldCount) / registrySize;

    if(doCleanFlag)
        cleanReservedMemory();
}


void DataTable::beginPK(unsigned int startMemPos, uint8_t pkFieldId, ...)
{
    if(pkFieldId < fieldCount)
    {
        va_list args;
        va_start(args, pkFieldId);

        primaryKey = pkFieldId;
        switch (fieldFormat[pkFieldId].type)
        {
            case DataTable_UINT8:
                *((uint8_t*)pKEmptyValue)       = (uint8_t) va_arg(args, int);
                break;
            case DataTable_ULONG:
                *((unsigned long*)pKEmptyValue) = va_arg(args, unsigned long);
                break;
            case DataTable_INT:
                *((int*)pKEmptyValue)           = va_arg(args, int);
                break;
            case DataTable_UINT:
                *((unsigned int*)pKEmptyValue)  = va_arg(args, unsigned int);
                break;
            case DataTable_FLOAT:
                *((float*)pKEmptyValue)         = (float) va_arg(args, double);
                break;
        default:
            break;
        }
        va_end(args);

        begin(startMemPos);
    }
    else
    {
        PRINT("BAD PK");
    }
}

void DataTable::delRegistry(int index)
{
    if(index < maxRegistries && index >= 0)
    {
        uint8_t pksize = fieldFormat[primaryKey].size();

        uint8_t fieldMemOffset = 0;
        for (uint8_t i = 0; i < primaryKey; i++)
            fieldMemOffset += fieldFormat[i].size();

        for (uint8_t b = 0; b < pksize; b++)
        {
            writeHard(startTable + fieldMemOffset + (registrySize*index) + b, pKEmptyValue[b]);
        }
    }
}

bool DataTable::available(int index)
{
    if(index < maxRegistries && index >= 0)
    {
        uint8_t pksize = fieldFormat[primaryKey].size();

        uint8_t fieldMemOffset = 0;
        for (uint8_t i = 0; i < primaryKey; i++)
            fieldMemOffset += fieldFormat[i].size();

        uint8_t data[MAX_VAR_SIZE];

        for (uint8_t b = 0; b < pksize; b++)
            data[b] = readHard(startTable + fieldMemOffset + (registrySize*index) + b);

        if(memcmp(pKEmptyValue, data, pksize) == 0)
        {
            return true;
        }
    }
    return false;
}

void DataTable::clean()
{

    for (unsigned int i = 0; i < lenght(); i++)
    {
        uint8_t pksize = fieldFormat[primaryKey].size();

        uint8_t fieldMemOffset = 0;
        for (uint8_t i = 0; i < primaryKey; i++)
            fieldMemOffset += fieldFormat[i].size();

        for (uint8_t b = 0; b < pksize; b++)
        {
            writeHard(startTable + fieldMemOffset + (registrySize*i) + b, pKEmptyValue[b]);
        }
    }
}

void DataTable::setRegistry(int index, ...)
{
    if(index < maxRegistries && index >= 0)
    {
        va_list args;
        va_start(args, index);

        unsigned int hardPos = startTable + (registrySize * index);
        
        for (uint8_t i = 0; i < fieldCount; i++)
        {
            uint8_t data[MAX_VAR_SIZE];

            switch (fieldFormat[i].type)
            {
                case DataTable_UINT8:
                    *((uint8_t*)data)       = (uint8_t) va_arg(args, int);
                    break;
                case DataTable_ULONG:
                    *((unsigned long*)data) = va_arg(args, unsigned long);
                    break;
                case DataTable_INT:
                    *((int*)data)           = va_arg(args, int);
                    break;
                case DataTable_UINT:
                    *((unsigned int*)data)  = va_arg(args, unsigned int);
                    break;
                case DataTable_FLOAT:
                    *((float*)data)         = (float) va_arg(args, double);
                    break;
            default:
                break;
            }

            uint8_t fieldSize = fieldFormat[i].size();
            for (uint8_t b = 0; b < fieldSize; b++)
                writeHard(hardPos++, data[b]);
        }
        va_end(args);
    }
}

void DataTable::getRegistry(int index, ...)
{
    if(index < maxRegistries && index >= 0)
    {
        va_list args;
        va_start(args, index);

        unsigned int hardPos = startTable + (registrySize * index);
        
        for (uint8_t i = 0; i < fieldCount; i++)
        {
            uint8_t data[MAX_VAR_SIZE];
            void* ptr = va_arg(args, void*);

            uint8_t fieldSize = fieldFormat[i].size();

            for (uint8_t b = 0; b < fieldSize; b++)
                data[b] = readHard(hardPos++);

            memcpy(ptr, data, fieldSize); // ptr = dest, data = source
        }
        va_end(args);
    }
}

int DataTable::findValueReferenced(uint8_t fieldIndex, void* query)
{
    if(fieldIndex < fieldCount)
    {
        uint8_t fieldSize = fieldFormat[fieldIndex].size();
        uint8_t data[MAX_VAR_SIZE];

        uint8_t fieldMemOffset = 0;
        for (uint8_t i = 0; i < fieldIndex; i++)
            fieldMemOffset += fieldFormat[i].size();    

        unsigned int hardPos = startTable;
        for (unsigned int i = 0; i < maxRegistries; i++)
        {
            for (uint8_t b = 0; b < fieldSize; b++)
                data[b] = readHard(hardPos + fieldMemOffset + b);

            hardPos += registrySize;

            if(memcmp(query, data, fieldSize) == 0)
            {
                return i;
            }
        }
    }
    return -1; 
}

int DataTable::findValue(uint8_t fieldIndex, ...)
{
    if(fieldIndex < fieldCount)
    {
        va_list args;
        va_start(args, fieldIndex);

        uint8_t query[MAX_VAR_SIZE];
        switch (fieldFormat[fieldIndex].type)
        {
            case DataTable_UINT8:
                *((uint8_t*)query)       = (uint8_t) va_arg(args, int);
                break;
            case DataTable_ULONG:
                *((unsigned long*)query) = va_arg(args, unsigned long);
                break;
            case DataTable_INT:
                *((int*)query)           = va_arg(args, int);
                break;
            case DataTable_UINT:
                *((unsigned int*)query)  = va_arg(args, unsigned int);
                break;
            case DataTable_FLOAT:
                *((float*)query)         = (float) va_arg(args, double);
                break;
        default:
            break;
        }
        va_end(args);

        return findValueReferenced(fieldIndex, query);
    }
    return -1; 
}


int DataTable::newPos()
{
    return findValueReferenced(primaryKey, pKEmptyValue);
}


// PRIVATE  FUNCTIONS

void DataTable::cleanReservedMemory()
{
    PRINT("Cleaning memory...");

    for (uint8_t i = 0; i < fieldCount; i++)
        writeHard(startMemory + i, fieldFormat[i].type);
    
    clean();
}