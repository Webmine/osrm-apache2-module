#include "socket/UnixServer.h"
#include "../include/ini-reader/INIReader.h"

int main(int argc, char **argv)
{
    UnixServer server = UnixServer();
    server.run();
}
