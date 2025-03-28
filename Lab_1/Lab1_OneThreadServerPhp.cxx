#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <err.h>
#include <cstring>
#include <pthread.h>
#include <iostream>

char myResponse[] = "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html; charset=UTF-8\r\n\r\n"
    "<!DOCTYPE html><html><head><title>Bye-bye baby bye-bye</title>"
    "<style>body { background-color: #111 }"
    "h1 { font-size:4cm; text-align: center; color: black;"
    " text-shadow: 0 0 2mm red}</style></head>"
    "<body><h1>Goodbye, world!</h1></body></html>\r\n";

int main() {
    int one = 1, client_fd;
    struct sockaddr_in svr_addr, cli_addr;
    socklen_t sin_len = sizeof (cli_addr);

    int sock = socket (AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        err (1, "can't open socket");
    }

    setsockopt (sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int));

    int port = 8080;
    svr_addr.sin_family = AF_INET;
    svr_addr.sin_addr.s_addr = INADDR_ANY;
    svr_addr.sin_port = htons (port);

    if (bind (sock, (struct sockaddr *) &svr_addr, sizeof (svr_addr)) == -1) {
        close (sock);
        err (1, "Can't bind");
    }

    listen (sock, 5);
    while (1) {
        client_fd = accept (sock, (struct sockaddr *) &cli_addr, &sin_len);
        printf ("got connection\n");

        if (client_fd == -1) {
            perror ("Can't accept");
            continue;
        }

        FILE* pipe = popen ("php -r 'echo phpversion();'", "r");
        char myResponse[512];
        char php_version[128];
        if (fscanf (pipe, "%s", php_version)) 
        {
            pclose (pipe);
            sprintf (myResponse, "HTTP/1.1 200 OK\r\n"
                    "Content-Type: text/html; charset=UTF-8\r\n\r\n"
                    "<!DOCTYPE html><html><head><title>Bye-bye baby bye-bye</title>"
                    "<style>body { background-color: #111 }"
                    "h1 { font-size:4cm; text-align: center; color: black;"
                    " text-shadow: 0 0 2mm red}</style></head>"
                    "<body><h1>Goodbye, world!\n PHP: %s</h1></body></html>\r\n", 
                    php_version);
            write (client_fd, myResponse, strlen (myResponse) * sizeof (char));
        } else {
            perror ("Can't run php.");
        }
        close (client_fd);
    }

    return 0;
}