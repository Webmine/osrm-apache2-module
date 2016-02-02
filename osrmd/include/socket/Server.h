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
#include <queue>

#include "BaseThread.h"
#include "osrm/engine_config.hpp"

using namespace osrm;
using namespace osrm::engine;

using namespace std;


class Server
{
public:
    Server();
    virtual ~Server();

    void run(int pool_size, EngineConfig& config);
    string get_request(int);
    bool send_response(int, string);
    void OnThreadFinished(BaseThread* thread);

protected:
    virtual void create();
    virtual void close_socket();
    void serve();

    queue<BaseThread*> idleThreads;
    queue<int> waitingClients;
    pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER; //mutex for clientqueue lock
    pthread_mutex_t mtx2 = PTHREAD_MUTEX_INITIALIZER; //mutex for clientqueue lock
    int server_;
    int buflen_;
    int pool_size_;
    char* buf_;
};
