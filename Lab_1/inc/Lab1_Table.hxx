#ifndef _Table_HeaderFile
#define _Table_HeaderFile

#include <vector>
#include <string>


class Table {
public:
    inline void setHeaders (const std::vector <std::string>& theHeader) { myHeaders = theHeader; }

    inline void addRow (const std::vector<std::string>& theRow) { myData.emplace_back (theRow); }

    void Print() const;

private:
    void PrintTableLine() const;

    std::vector<std::vector<std::string>> myData;
    std::vector<std::string> myHeaders;
};

#endif
