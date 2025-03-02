#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <err.h>
#include <pthread.h>
#include <string.h>

#include "WebServers.hxx"

namespace parallel {
namespace {
struct ResponseData {
    int myClientFd;
    const std::string* myResponse;
};

std::string GetPhpVersion()
{
    FILE* aFilePointer = popen("php -r 'echo phpversion();'", "r");
    if (!aFilePointer) return "Unknown";
    
    char aBuffer[128];
    std::string aPhpVersion;
    while (fgets(aBuffer, sizeof(aBuffer), aFilePointer) != nullptr) {
        aPhpVersion += aBuffer;
    }
    pclose(aFilePointer);

    return aPhpVersion;
}

void* HandleClient(void* theArg) {
    ResponseData* aResponseData = static_cast<ResponseData*>(theArg);

    write(aResponseData->myClientFd, aResponseData->myResponse->c_str(), aResponseData->myResponse->size());

    close(aResponseData->myClientFd);

    delete aResponseData;

    return nullptr;
}

void RunServer (const char* theResponse, bool includePhpVersion)
{
    int one = 1, client_fd;
    struct sockaddr_in svr_addr, cli_addr;
    socklen_t sin_len = sizeof(cli_addr);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    err(1, "can't open socket");

    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int));

    int port = 8080;
    svr_addr.sin_family = AF_INET;
    svr_addr.sin_addr.s_addr = INADDR_ANY;
    svr_addr.sin_port = htons(port);

    if (bind(sock, (struct sockaddr *) &svr_addr, sizeof(svr_addr)) == -1) {
        close(sock);
        err(1, "Can't bind");
    }

    listen(sock, 5);
    while (1) {
        client_fd = accept(sock, (struct sockaddr *) &cli_addr, &sin_len);
        printf("got connection\n");

        if (client_fd == -1) {
            perror("Can't accept");
            continue;
        }

        std::string aResponse (theResponse);
        if (includePhpVersion) {
            aResponse += "<p>PHP Version: " + GetPhpVersion() + "</p>";
        }
        aResponse += "</body></html>\r\n";

        write (client_fd, aResponse.c_str(), strlen (aResponse.c_str()) - 1);
        close(client_fd);
    }
}

void RunMultithreadingServer(const std::string& theResponse, bool includePhpVersion)
{
    int one = 1;
    struct sockaddr_in svr_addr, cli_addr;
    socklen_t sin_len = sizeof(cli_addr);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) err(1, "can't open socket");

    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int));

    int port = 8080;
    svr_addr.sin_family = AF_INET;
    svr_addr.sin_addr.s_addr = INADDR_ANY;
    svr_addr.sin_port = htons(port);

    if (bind(sock, (struct sockaddr*) &svr_addr, sizeof(svr_addr)) == -1) {
        close(sock);
        err(1, "Can't bind");
    }

    pthread_attr_t thread_attr;
    if (pthread_attr_init(&thread_attr) != 0) {
        perror("Cannot initialize thread attributes");
        exit(EXIT_FAILURE);
    }

    if (pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED) != 0) {
        perror("Cannot set detached state");
        exit(EXIT_FAILURE);
    }

    if (pthread_attr_setstacksize(&thread_attr, 512 * 1024) != 0) {
        perror("Cannot set stack size");
        exit(EXIT_FAILURE);
    }

    listen(sock, 5);

    while (1) {
        int client_fd = accept(sock, (struct sockaddr*) &cli_addr, &sin_len);
        printf("got connection\n");

        if (client_fd == -1) {
            perror("Can't accept");
            continue;
        }

        std::string aResponse = theResponse;
        if (includePhpVersion) {
            aResponse += "<p>PHP Version: " + GetPhpVersion() + "</p>";
        }
        aResponse += "</body></html>\r\n";

        ResponseData* aResponseData = new ResponseData;
        aResponseData->myClientFd = client_fd;
        aResponseData->myResponse = &aResponse;

        pthread_t thread;
        if (pthread_create(&thread, &thread_attr, HandleClient, aResponseData) != 0) {
            perror("pthread_create failed");
            close(client_fd);
            delete aResponseData;
        }
    }
}

}

void WebServers::OneThreadServer()
{
    RunServer (myResponse, false);
}

void WebServers::OneThreadServerWithPhp() 
{
    RunServer (myResponse, true);
}

void WebServers::MultiThreadServer()
{
    RunMultithreadingServer (myResponse, false);
}

void WebServers::MultiThreadServerWithPhp()
{
    RunMultithreadingServer (myResponse, true);
}

}
