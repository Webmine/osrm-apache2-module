#include "socket/UnixServer.h"

//const char* UnixServer::socket_name_ = "asd";

UnixServer::UnixServer(std::string socket_name)
: socket_name(socket_name.c_str())
{

}

UnixServer::~UnixServer()
{
    unlink(socket_name);
}

void UnixServer::create()
{
    struct sockaddr_un server_addr;

    // setup socket address sructure
    bzero(&server_addr,sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path,socket_name,sizeof(server_addr.sun_path) - 1);

    // create socket
    server_ = socket(PF_UNIX,SOCK_STREAM,0);
    if (!server_)
    {
        perror("socket");
        exit(-1);
    }

    // call bind to associate the socket with the UNIX file system
    if (bind(server_,(const struct sockaddr *)&server_addr,sizeof(server_addr)) < 0)
    {
        perror("bind");
        exit(-1);
    }

      // convert the socket to listen for incoming connections
    if (listen(server_,SOMAXCONN) < 0)
    {
        perror("listen");
        exit(-1);
    }
}

void UnixServer::close_socket()
{
    unlink(socket_name);
}
