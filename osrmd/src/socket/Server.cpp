#include "socket/Server.h"
#include "ClientThread.h"
#include <iostream>
#include "osrm/osrm.hpp"
#include "logger/log.h"

Server::Server()
{

}

Server::~Server()
{
    delete buf_;
}

void Server::run(int pool_size, EngineConfig& config)
{
    // setup variables
    buflen_ = 1024;
    buf_ = new char[buflen_+1];

    for(int i = 0; i < pool_size; i++)
    {
        idleThreads.push((BaseThread*)new ClientThread(this, i, config));
    }
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

        if(idleThreads.size() > 0)
        {
            ClientThread* worker = (ClientThread*)idleThreads.front();
            idleThreads.pop();
            worker->Prepare(client);
            worker->StartInternalThread();
        }
        else
        {
            waitingClients.push(client);
        }

        LOG_DEBUG("Available workers: %d", idleThreads.size());
        LOG_DEBUG("Client queue: %d", waitingClients.size());
    }

    close_socket();
}

void Server::OnThreadFinished(BaseThread* thread)
{
    ClientThread* worker = ((ClientThread*)thread);

    LOG_DEBUG("Worker %d finished", worker->threadNum);

    worker->Reset();

    pthread_mutex_lock(&mtx);
    int waiting = waitingClients.size();
    pthread_mutex_unlock(&mtx);

    LOG_DEBUG("Client queue length: %d", waiting);

    //if there is client waiting for free worker assign this thread to it
    if(waiting > 0)
    {
        LOG_DEBUG("Worker %d is going to work with queued client", worker->threadNum);

        pthread_mutex_lock(&mtx2);
        LOG_DEBUG("Worker %d locked the mutex", worker->threadNum);
        int client = waitingClients.front();
        LOG_DEBUG("Worker %d got a client", worker->threadNum);
        waitingClients.pop();
        LOG_DEBUG("Worker %d removed client from queue", worker->threadNum);
        pthread_mutex_unlock(&mtx2);

        LOG_DEBUG("Worker %d assigned to client: %d", worker->threadNum, client);

        worker->Prepare(client);
        worker->StartInternalThread();
    }
    else // if no more clients waiting, put this thread back to the pool
    {
        LOG_DEBUG("Worker %d is now free", worker->threadNum);
        idleThreads.push(thread);
    }
}

string Server::get_request(int client)
{
    string request = "";

    int nread = recv(client,buf_,1024,0);

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
    request.append(buf_,nread);

    return request;
}

bool Server::send_response(int client, string response)
{
    // prepare to send response
    const char* ptr = response.c_str();
    int nleft = response.length();
    int nwritten;
    // loop to be sure it is all sent
    while (nleft)
    {
        if ((nwritten = send(client, ptr, nleft, 0)) < 0)
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
                return false;
            }
        }
        else if (nwritten == 0)
        {
            // the socket is closed
            return false;
        }
        nleft -= nwritten;
        ptr += nwritten;
    }
    return true;
}
