#include "socket/Server.h"
#include "WorkerQueue.h"
#include <iostream>
#include "osrm/osrm.hpp"
#include "logger/log.h"

Server::Server()
{

}

Server::~Server()
{
    workers->abort();
    workers->waitForCompletion();
}

void Server::run(int pool_size  /*, EngineConfig& config*/)
{
    workers = new WorkQueue(pool_size);
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
    string request = "";

    char buffer [1025];
    int nread = recv(client,buffer,1024,0);

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
    // be sure to use append in case we have binary data
    request.append(buffer,nread);

    return request;
}

void Server::send_response(int client, const char* response)
{
    // prepare to send response
    int nleft = strlen(response);
    int nwritten;
    // loop to be sure it is all sent
    while (nleft)
    {
        if ((nwritten = send(client, response, nleft, 0)) < 0)
        {
            if (errno == EINTR)
            {
                // the socket call was interrupted -- try again
                continue;
            }
            else
            {
                // an error occurred, so break out
                LOG_ERROR("write error");
                return;
            }
        }
        else if (nwritten == 0)
        {
            // the socket is closed
            return;
        }
        nleft -= nwritten;
        response += nwritten;
    }
    return;
}
