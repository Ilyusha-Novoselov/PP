#include <iostream>
#include <iomanip>

#include "Lab2_Table.hxx"

void Table::PrintInTerminal() const 
{
    if (myHeaders.empty()) {
        std::cout << "Table has no headers!" << std::endl;
        return;
    }

    PrintTableLine();
    for (const auto& aHeader : myHeaders) {
        std::cout << "| " << std::left << std::setw(15) << aHeader;
    }
    std::cout << "|" << std::endl;
    PrintTableLine();

    for (const auto& aRow : myData) {
        for (size_t i = 0; i < aRow.size(); ++i) {
            std::cout << "| " << std::left << std::setw(15) << aRow[i];
        }
        std::cout << "|" << std::endl;
    }

    PrintTableLine();
}

void Table::PrintForWord() const 
{
    if (myHeaders.empty()) {
        std::cout << "Table has no headers!" << std::endl;
        return;
    }

    for (size_t i = 0; i < myHeaders.size(); ++i) {
        std::cout << myHeaders[i];
        if (i < myHeaders.size() - 1) {
            std::cout << ";";
        }
    }
    std::cout << "\n";

    for (const auto& row : myData) {
        for (size_t i = 0; i < row.size(); ++i) {
            std::cout << row[i];
            if (i < row.size() - 1) {
                std::cout << ";";
            }
        }
        std::cout << "\n";
    }
}

void Table::PrintTableLine() const 
{
    for (size_t i = 0; i < myHeaders.size(); ++i) {
        std::cout << "+-----------------";
    }
    std::cout << "+" << std::endl;
}