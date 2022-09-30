#include <iostream>
using namespace std;


#include "TESTDataTable.h"
TESTDataTable table(2, 100);


int main(int, char**) {

    table.setupField(0, DataTable_UINT8);
    table.setupField(1, DataTable_ULONG);

    table.begin(0);

    cout << "Table Len: " << table.lenght() << endl;

    table.setRegistry(0, 3, 0);
    table.setRegistry(1, 5, 0);
    table.setRegistry(2, 50, 42840868UL);
    table.setRegistry(5, 11, 2349123UL);
    table.setRegistry(8, 22, 4222222222UL);

    cout << "find idx: " << table.findValue(0, 0);

    cout << "\n\n";

    for (unsigned int i = 0; i < table.lenght(); i++)
    {
        uint8_t a;
        unsigned long b;
        table.getRegistry(i, &a, &b);
        char buf[48];
        sprintf(buf, "%05u: %u\t%lu\n", i, a, b);
        cout << buf;
    }
}
