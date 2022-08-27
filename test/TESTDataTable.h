#ifndef TESTDATATABLE_H
#define TESTDATATABLE_H

#include "../src/DataTable.h"

class TESTDataTable : public DataTable
{
public:
    TESTDataTable(uint8_t regs, unsigned int size) : DataTable(regs, size)
    {
        memory = new uint8_t[size];
    }
    ~TESTDataTable()
    {
        delete[] memory;
    }


private:
    // Wrappers for EEPROM, SPIFFS, etc
    virtual void beginHard(unsigned int hardMemSize)
    {
        ; // Nada
    }
    virtual void writeHard(unsigned int memPos, uint8_t data)
    {
        memory[startMemory + memPos] = data;
    }
    virtual uint8_t readHard(unsigned int memPos)
    {
        return memory[startMemory + memPos];
    }
    
    uint8_t* memory = NULL; // Hardware emulated memory
};

#endif