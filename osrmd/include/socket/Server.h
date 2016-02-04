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

#include "WorkerQueue.h"
#include "osrm/engine_config.hpp"

using namespace osrm;
using namespace osrm::engine;

using namespace std;


class Server
{
public:
    Server();
    virtual ~Server();

    void run(int pool_size/*, EngineConfig& config*/);
    static string get_request(int);
    static void send_response(int, const char*);

protected:
    virtual void create();
    virtual void close_socket();
    void serve();

    WorkQueue* workers;
    int server_;
    int pool_size_;
};
