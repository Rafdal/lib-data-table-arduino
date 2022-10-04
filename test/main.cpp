#include <iostream>
using namespace std;


#include "TESTDataTable.h"
TESTDataTable table(2, 90);


int main(int, char**)
{

    table.setupField(0, DataTable_ULONG);
    table.setupField(1, DataTable_UINT8);

    table.begin(0);

    cout << "Table Len: " << table.lenght() << endl;

    table.setRegistry(0, 23446, 3);
    table.setRegistry(1, 42840868UL, 50);

    table.setRegistry(7, 1111, 5);
    table.setRegistry(5, 2349123UL, 11);
    table.setRegistry(8, 4222222222UL, 22);


    cout << "TEST OUT: " << table.newPos() << endl;
    // table.setRegistry(table.newPos(), 99999UL, 64);
    // table.setRegistry(table.newPos(), 888888UL, 64);
    // table.setRegistry(table.newPos(), 777UL, 64);

    cout << "find idx: " << table.findValue(0, 1111) << endl;
    cout << "available: " << table.available(2) << endl;

    cout << "TABLE:\n";

    for (unsigned int i = 0; i < table.lenght(); i++)
    {
        uint8_t a;
        unsigned long num;
        table.getRegistry(i, &num, &a);
        char buf[48];
        sprintf(buf, "%4u:\t%u\t%lu\n", i, a, num);
        cout << buf;
    }

    cout << "\nfinished\n";
}
