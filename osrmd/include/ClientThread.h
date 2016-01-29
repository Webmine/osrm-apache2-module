#ifndef CLIENTTHREAD_H_INCLUDED
#define CLIENTTHREAD_H_INCLUDED

#include "thread.h"
#include "socket/server.h"

class ClientThread : public MyThreadClass
{
public:
    ClientThread(Server* server, int& client);
    ~ClientThread();
    void InternalThreadEntry();

private:
    Server* server;
    int client;
};

#endif // CLIENTTHREAD_H_INCLUDED
