#ifndef CLIENTTHREAD_H_INCLUDED
#define CLIENTTHREAD_H_INCLUDED

#include "BaseThread.h"
#include "socket/Server.h"


class ClientThread : public BaseThread
{
public:
    ClientThread(Server* server);
    ~ClientThread();
    void Prepare(int& newClient);
    void Reset();
    void InternalThreadEntry();

private:
    Server* server;
    int client;
};

#endif // CLIENTTHREAD_H_INCLUDED
