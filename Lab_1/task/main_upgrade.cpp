#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <err.h>
#include <iostream>
#include <cstring>
#include <pthread.h>
using namespace std;

const int BUF_SIZE = 512;
struct thread_data
{
    int client_fd;
};

void *thread_job(void *arg)
{
    thread_data* par = (struct thread_data*)arg;
    int client_fd = par->client_fd;
    FILE* pipe = popen("php -r 'echo phpversion();'", "r");
    char response[BUF_SIZE];
    char php_version[128];
    if (fscanf(pipe, "%s", php_version)) 
    {
        pclose(pipe);
        sprintf(response, "HTTP/1.1 200 OK\r\n"
                  "Content-Type: text/html; charset=UTF-8\r\n\r\n"
                  "<!DOCTYPE html><html><head><title>Bye-bye baby bye-bye</title>"
                  "<style>body { background-color: #111 }"
                  "h1 { font-size:4cm; text-align: center; color: black;"
                  " text-shadow: 0 0 2mm red}</style></head>"
                  "<body><h1>Goodbye, world!\n PHP: %s</h1></body></html>\r\n", 
                  php_version);
        write(client_fd, response, strlen(response) * sizeof(char));
    }
    else {
        perror("Can't run php.");
    }
    close(client_fd);
    return NULL;
}

int main()
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

    if (bind(sock, (struct sockaddr*)&svr_addr, sizeof(svr_addr)) == -1) {
        close(sock);
        err(1, "Can't bind");
    }

    listen(sock, 5);
    int err;
    pthread_attr_t thread_attr;
    err = pthread_attr_init(&thread_attr);
    if (err != 0) {
        perror("Cannot init attr for thread");
        exit(-1);
    }

    err = pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
    if (err != 0) {
        perror("Cannot create deteched thread");
        exit(-1);
    }

    err = pthread_attr_setstacksize(&thread_attr, 512 * 1024);
    if (err != 0)
    {
        perror("Cannot create stack for thread");
        exit(-1);
    }

    while (1)
    {
        client_fd = accept(sock, (struct sockaddr*)&cli_addr, &sin_len);

        if (client_fd == -1)
        {
            perror("Can't accept");
            continue;
        }

        // Устанавливаем параметры потока.
        pthread_t thread;
        thread_data *data = new thread_data{client_fd};
        // Создаём поток.
        int err = pthread_create(&thread, NULL, thread_job, data);
        if (err != 0)
        {
            cout << "Cannot create a thread: " << strerror(err) << endl;
            close(client_fd);
            delete data;
            continue;
        }

        pthread_detach(thread);
        pthread_attr_destroy(&thread_attr);
    }
    pthread_exit(NULL);
}
