#include "socket/Server.h"
#include "ClientThread.h"
#include <iostream>

Server::Server()
{
    // setup variables
    buflen_ = 1024;
    buf_ = new char[buflen_+1];

    int numThreads = 10;
    for(int i = 0; i < numThreads; i++)
    {
        idleThreads.push((BaseThread*)new ClientThread(this, i));
    }
}

Server::~Server()
{
    delete buf_;
}

void Server::run()
{
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

        //cout << "DEBUG: Available workers: " << idleThreads.size();
        //cout << "DEBUG: Client queue: " << waitingClients.size();
    }

    close_socket();
}

void Server::OnThreadFinished(BaseThread* thread)
{
    ClientThread* worker = ((ClientThread*)thread);

    cout << "Worker "<< worker->threadNum <<" finished \n";


    worker->Reset();

    pthread_mutex_lock(&mtx);
    int waiting = waitingClients.size();
    pthread_mutex_unlock(&mtx);

    cout << "Client queue length: " << waiting << "\n";

    //if there is client waiting for free worker assign this thread to it
    if(waiting > 0)
    {
        cout << "Worker "<< worker->threadNum <<" is going to work with queued client \n";

        pthread_mutex_lock(&mtx2);
        cout << "Worker "<< worker->threadNum <<" locked the mutex\n";
        int client = waitingClients.front();
        cout << "Worker "<< worker->threadNum <<" got a client\n";
        waitingClients.pop();
        cout << "Worker "<< worker->threadNum <<" removed client from queue\n";
        pthread_mutex_unlock(&mtx2);

        cout << "Worker "<< worker->threadNum <<" assigned to client: " << client << "\n";

        worker->Prepare(client);
        worker->StartInternalThread();
    }
    else // if no more clients waiting, put this thread back to the pool
    {
        cout << "Worker "<< worker->threadNum <<" is now free" << "\n";
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
                perror("write");
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
