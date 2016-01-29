#pragma once

#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include <string>

using namespace std;

class Server
{
public:
    Server();
    ~Server();

    void run();
    string get_request(int);
    bool send_response(int, string);

protected:
    virtual void create();
    virtual void close_socket();
    void serve();
    //void* handle(void *arg);


    int server_;
    int buflen_;
    char* buf_;
};
