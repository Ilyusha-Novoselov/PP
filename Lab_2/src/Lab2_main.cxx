#include <array>
#include <iostream>

#include "Lab2_TaskManager.hxx"
#include "Lab2_MapReduce.hxx"
#include "Lab2_Table.hxx"
#include "Lab2_FileChecker.hxx"

namespace {
void CompareMutexSpin()
{
    std::array <size_t, 6> aTaskSizes = {50, 100, 500, 1000, 5000, 10000};
    Table aTable;
    for (size_t aThreads = 1; aThreads <= 16; aThreads *= 2) {
        for (size_t aTaskSize : aTaskSizes) {
            parallel::TaskManager::EstimatePrimitiveSynh (aTaskSize, aThreads, true, aTable);
            parallel::TaskManager::EstimatePrimitiveSynh (aTaskSize, aThreads, false, aTable);
        }
    }

    aTable.PrintInTerminal();
    aTable.PrintForWord();
}

}

int main()
{
    system("clear");
    int aChoice;

    std::cout << "1. Print info about thread/task with Mutex\n";
    std::cout << "2. Print info about thread/task without Mutex\n";
    std::cout << "3. Compare Mutex/SpinLock\n";
    std::cout << "4. Simulate Condition Var\n";
    std::cout << "5. MapReduce\n";
    std::cout << "6. FileChecker\n";
    std::cout << "0. Exit\n";
    std::cout << "Enter your choice: ";
    std::cin >> aChoice;

    switch(aChoice) {
        case 1: {
            parallel::TaskManager::CheckTheExample (true);
            break;
        }
        case 2: {
            parallel::TaskManager::CheckTheExample (false);
            break;
        }
        case 3: {
            CompareMutexSpin();
            break;
        }
        case 4: {
            parallel::TaskManager::SimulateCondVar();
            break;
        }
        case 5: {
            parallel::MapReduce::TestMapReduce();
            break;
        }
        case 6: {
            parallel::FileChecker::TestFileChecker();
            break;
        }
    }

    return 0;
}