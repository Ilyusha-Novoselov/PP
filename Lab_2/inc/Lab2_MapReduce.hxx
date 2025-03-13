#ifndef _Lab2_MapReduce_HeaderFile
#define _Lab2_MapReduce_HeaderFile

#include <vector>
#include <functional>

class Table;

namespace parallel {

class MapReduce {
public:
    static void StartMapReduce (Table& theTable, size_t theNumOfThreads = 4, size_t theArraySize = 10);

    static void TestMapReduce();

private:
    static int DoMapReduce (std::vector <int>& data, 
                            std::function <int (int)> mapFunc, 
                            std::function <int (int, int)> reduceFunc,
                            int numThreads);
    

};

}

#endif
