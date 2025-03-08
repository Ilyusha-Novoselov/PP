#ifndef _WebServers_HeaderFile
#define _WebServers_HeaderFile

#include <string>

namespace parallel {

class WebServers {
public:
    static void OneThreadServer();

    static void OneThreadServerWithPhp();
    
    static void MultiThreadServer();
    
    static void MultiThreadServerWithPhp();
};

}

#endif