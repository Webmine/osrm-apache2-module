#include "socket/Server.h"
#include "WorkerQueue.h"
#include <iostream>
#include "osrm/osrm.hpp"
#include "logger/log.h"
#include <cstring>

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
    int requestLength = atoi(Server::get_bytes(client, 4));
    char* request = Server::get_bytes(client, requestLength);
    std::string str(request);
    return str;
}

char* Server::get_bytes(int client, int length)
{

    char buffer [length];
    int nread = recv(client,buffer,length,0);

    if (nread < 0)
    {
        //cout << "DEBUG: connection closed" << client;
        return "";
    }
    else if (nread == 0)
    {
        // the socket is closed
        //cout << "DEBUG: connection closed" << client;
        return "";
    }

    return buffer;
}

void Server::send_response(int client, const char* response)
{
    // prepare to send response
    int nleft = strlen(response);

    char* buffer = new char[4+nleft];
    sprintf(buffer,"%d",nleft);
    strcat(buffer, response);


    nleft += 4;
    int nwritten;
    // loop to be sure it is all sent
    while (nleft)
    {
        if ((nwritten = send(client, buffer, nleft, 0)) < 0)
        {
            if (errno == EINTR)
            {
                LOG_TRACE("Socket write interrupted on client %d, retrying...", client);
                // the socket call was interrupted -- try again
                continue;
            }
            else
            {
                // an error occurred, so break out
                LOG_ERROR("Socket write error on client %d", client);
                return;
            }
        }
        else if (nwritten == 0)
        {
            // the socket is closed
            LOG_DEBUG("Client %d disconnected.", client);
            return;
        }
        nleft -= nwritten;
        buffer += nwritten;
    }
    delete buffer;
    return;
}
