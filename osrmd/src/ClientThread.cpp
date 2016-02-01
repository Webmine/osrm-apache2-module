#include "ClientThread.h"
#include <iostream>

ClientThread::ClientThread(Server* server)
{
    this->server = server;
}

ClientThread::~ClientThread()
{
}

void ClientThread::Prepare(int& newClient)
{
    client = newClient;
}

void ClientThread::Reset()
{
    client = 0;
}

void ClientThread::InternalThreadEntry()
{
    //just one request, then response and byebye...
    //while (1)
    //{
        // get a request
        string request = server->get_request(client);
        std::cout << "DEBUG: Incoming:" << request;

        if (!request.empty())
        {
            // send response
            bool success = server->send_response(client,request);
        }
    //}
    close(client);
    server->OnThreadFinished((BaseThread*)this);
}

