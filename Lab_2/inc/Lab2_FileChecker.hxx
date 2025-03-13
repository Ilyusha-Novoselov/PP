#ifndef _Lab2_FileChecker_HeaderFile
#define _Lab2_FileChecker_HeaderFile

#include <string>

class Table;

namespace parallel {

class FileChecker {
public:
    static void TraverseDirectory (const std::string& thePath, const std::string& theSubstr);

    static void StartFileChecker (size_t theNumberOfThreads, 
                                  std::string& thePath, 
                                  std::string& theSubstr, 
                                  Table& theTable);

    static void TestFileChecker();

};

}

#endif
