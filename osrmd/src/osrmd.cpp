#include "socket/UnixServer.h"
#include "ini-reader/INIReader.h"
#include "logger/log.h"
#include <string>
#include <iostream>

void interrupt(int);
UnixServer* server;

int main(int argc, char **argv)
{
    // setup handler for Control-C so we can properly unlink the UNIX
    // socket when that occurs
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = interrupt;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

    string iniFilePath;
    if(argc > 1 && argc < 4 && strncmp(argv[1], "-c",2) == 0)
    {
        iniFilePath = argv[3];
    }
    else
    {
        iniFilePath = "osrmd.ini";
    }
    INIReader reader(iniFilePath);

    if (reader.ParseError() < 0) {
        std::cout << "Can't load " << iniFilePath << std::endl;
        return 1;
    }

    // read config vaules
    // logger
    int log_level = reader.GetInteger("log","level",4); //default = warn
    string log_file = reader.Get("log","file_name","osrmd");
    string log_dir = reader.Get("log","directory","./log");
    // socket
    string socket_name = reader.Get("socket","name","/tmp/unix-socket");
    // threading
    int thread_pool_size = reader.GetInteger("threading","pool_size",10);
    // osrm
    string osrm_file_name = reader.Get("osrm","file_name","UNDEFINED");

    // init logger
    log_init((LogLevel)log_level, log_file.c_str(), log_dir.c_str());

    /*
    //log examples

    LOG_NOTICE("%s [time:%d]", "test calling log", time(NULL));
    LOG_DEBUG("debug msg,only write to log when the loglevel bigger than or equal to debug [time:%d]", time(NULL));
    LOG_WARN("warnning msg will be writing to the error files [time:%d]", time(NULL));
    LOG_ERROR("you also can change  number of output files by rewrite the macro_define.h");

    */

    server = new UnixServer(socket_name);
    server->run(thread_pool_size);

    delete server;
}

void interrupt(int)
{
    server->close_socket();
}
