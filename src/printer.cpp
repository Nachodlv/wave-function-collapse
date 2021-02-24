#include "../include/printer.h"

#include <iostream>
#include <ostream>

Printer::Printer(): h_console(GetStdHandle(STD_OUTPUT_HANDLE))
{
}

void Printer::test_colors() const
{
    for (int k = 1; k < 255; k++)
    {
        SetConsoleTextAttribute(h_console, k);
        std::cout << k << " Test color" << std::endl;
    }
}
