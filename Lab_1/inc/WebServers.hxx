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

private:
    static constexpr const char myResponse[] =
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html; charset=UTF-8\r\n\r\n"
    "<!DOCTYPE html><html><head><title>Bye-bye baby bye-bye</title>"
    "<style>body { background-color: #111 }"
    "h1 { font-size:4cm; text-align: center; color: black;"
    " text-shadow: 0 0 2mm red}</style></head>"
    "<body><h1>Goodbye, world!</h1></body></html>\r\n";
};

}

#endif