#include "socket/Server.h"
#include "WorkerQueue.h"
#include <iostream>
#include "osrm/osrm.hpp"
#include "logger/log.h"
#include <cstring>
#include <netinet/in.h>

Server::Server()
{

}

Server::~Server()
{
    workers->abort();
    workers->waitForCompletion();
}

void Server::run(int pool_size , EngineConfig& config)
{
    workers = new WorkQueue(config, pool_size);
    // create and run the server
    create();
    serve();
}

void Server::create()
{
}

void Server::close_socket()
{
}

void Server::serve()
 {
    // setup client
    int client;
    struct sockaddr_in client_addr;
    socklen_t clientlen = sizeof(client_addr);

      // accept clients
    while ((client = accept(server_,(struct sockaddr *)&client_addr,&clientlen)) > 0)
    {

        workers->submit(client);

//        LOG_DEBUG("Available workers: %d", idleThreads.size());
//        LOG_DEBUG("Available workres queue size: %d", sizeof(idleThreads));
//        LOG_DEBUG("Client queue: %d", waitingClients.size());
//        LOG_TRACE("Client queue size: %d bytes", waitingClients.size() * 4);

    }

    close_socket();
}


string Server::get_request(int client)
{
    int length = 0;
    Server::get_bytes(client, &length, sizeof(int));
    int requestLength = ntohl(length);
    char* request = new char[requestLength];

    Server::get_bytes(client,request, requestLength);
    std::string str(request);

    delete request;
    return str;
}

void Server::get_bytes(int client,void* buffer, int length)
{
    int nread = recv(client,buffer,length,0);

    if (nread < 0)
    {
        //cout << "DEBUG: connection closed" << client;
        return;
    }
    else if (nread == 0)
    {
        // the socket is closed
        //cout << "DEBUG: connection closed" << client;
        return;
    }

    return;
}

void Server::send_bytes(int client, const void* bytes, int length)
{
    // prepare to send request
    int nleft = length;


    int nwritten;
    // loop to be sure it is all sent
    while (nleft)
    {
        if ((nwritten = send(client, bytes, nleft,0)) < 0)
        {
            if (errno == EINTR)
            {
                // the socket call was interrupted -- try again
                continue;
            }
            else
            {
                // an error occurred, so break out
                return;
            }
        }
        else if (nwritten == 0)
        {
            // the socket is closed
            return;
        }
        nleft -= nwritten;
        bytes += nwritten;
    }
    return;
}

void Server::send_response(int client, const char* response)
{
    // prepare to send response
    int responseLength = strlen(response);

    int convertedLength = htonl(responseLength);
    Server::send_bytes(client, &convertedLength, sizeof(convertedLength));

    Server::send_bytes(client, response, responseLength);

}
