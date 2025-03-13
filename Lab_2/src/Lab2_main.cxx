#include <array>

#include "Lab2_TaskManager.hxx"
#include "Lab2_Table.hxx"

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
}

}

int main()
{
    CompareMutexSpin();

    return 0;
}