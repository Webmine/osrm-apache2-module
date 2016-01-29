#include "ClientThread.h"
#include <iostream>

ClientThread::ClientThread(Server* server, int& client)
{
    this->server = server;
    this->client = client;
}

ClientThread::~ClientThread()
{
}

void ClientThread::InternalThreadEntry()
{
    // loop to handle all requests
    while (1)
    {
        // get a request
        string request = server->get_request(client);
        // break if client is done or an error occurred
        if (request.empty())
            break;

        std::cout << "DEBUG: Incoming:" << request;

        // send response
        bool success = server->send_response(client,request);
        // break if an error occurred
        if (not success)
            break;
    }
    close(client);
}

