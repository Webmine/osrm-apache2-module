#pragma once

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/un.h>

#include "Server.h"

class UnixServer : public Server
{

public:
    UnixServer(std::string socket_name);
    ~UnixServer();
    void close_socket();

protected:
    void create();

private:
    const char* socket_name;
};
