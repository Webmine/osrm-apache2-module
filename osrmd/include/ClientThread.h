#ifndef CLIENTTHREAD_H_INCLUDED
#define CLIENTTHREAD_H_INCLUDED

#include "BaseThread.h"
#include "socket/Server.h"
#include "osrm/osrm.hpp"
#include "osrm/engine_config.hpp"

using namespace osrm;
using namespace osrm::engine;

class ClientThread : public BaseThread
{
public:
    ClientThread(Server* server, int number, EngineConfig& config);
    ~ClientThread();
    void Prepare(int& newClient);
    void Reset();
    void InternalThreadEntry();

    int threadNum;

private:
    void RunOsrmQuery();
    Server* server;
    osrm::engine::EngineConfig engineConfig;
    osrm::OSRM* osrmEngine;
    int client;
};

#endif // CLIENTTHREAD_H_INCLUDED
