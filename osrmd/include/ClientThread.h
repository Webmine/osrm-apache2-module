#ifndef CLIENTTHREAD_H_INCLUDED
#define CLIENTTHREAD_H_INCLUDED

#include "BaseThread.h"
#include "socket/Server.h"
#include "osrm/osrm.hpp"


class ClientThread : public BaseThread
{
public:
    ClientThread(Server* server, int number);
    ~ClientThread();
    void Prepare(int& newClient);
    void Reset();
    void InternalThreadEntry();

    int threadNum;

private:
    void RunOsrmQuery();
    Server* server;
    osrm::OSRM* osrmEngine;
    int client;
};

#endif // CLIENTTHREAD_H_INCLUDED
