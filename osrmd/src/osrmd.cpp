#include "socket/UnixServer.h"
#include "ini-reader/INIReader.h"
#include "logger/log.h"
#include "osrm_config_helper.h"
#include "osrm/engine_config.hpp"
#include <string>
#include <iostream>
#include <signal.h>
#include <exception>

void interrupt(int);
UnixServer* socketServer;

int main(int argc, char **argv) try
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
        iniFilePath = "/etc/osrmd/osrmd.ini";
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
    string log_dir = reader.Get("log","directory","/var/log/osrmd");
    bool log_console = reader.GetBoolean("log", "console_logging", true);
    // socket
    string socket_name = reader.Get("socket","name","/tmp/osrmd.sock");
    // threading
    int thread_pool_size = reader.GetInteger("threading","pool_size",10);
    // osrm
    string osrm_file_name = reader.Get("osrm","file_name","UNDEFINED");
    bool osrm_use_shared_memory = reader.GetBoolean("osrm", "use_shared_memory", true);

    // init logger
    log_init((LogLevel)log_level, log_file.c_str(), log_dir.c_str(), log_console);

    /*
    //log examples

    LOG_NOTICE("%s [time:%d]", "test calling log", time(NULL));
    LOG_DEBUG("debug msg,only write to log when the loglevel bigger than or equal to debug [time:%d]", time(NULL));
    LOG_WARN("warnning msg will be writing to the error files [time:%d]", time(NULL));
    LOG_ERROR("you also can change  number of output files by rewrite the macro_define.h");

    */

    LOG_NOTICE("USING OSRM FILE: %s", osrm_file_name.c_str());

    EngineConfig config = EngineConfig();

    if(osrm_use_shared_memory)
    {
        config.use_shared_memory = true;
    }
    else
    {
        config.use_shared_memory = false;
        if(populate_server_paths(osrm_file_name, config.server_paths) == 1)
        {
            LOG_ERROR("Error loading osrm files.");
            return 1;
        }
    }

    socketServer = new UnixServer(socket_name);
    socketServer->run(thread_pool_size, config);

    socketServer->close_socket();
    delete socketServer;
}
catch(const std::exception &e)
{
    LOG_ERROR("Exception occured: %s", e.what());
    socketServer->close_socket();
    delete socketServer;
}

void interrupt(int)
{
    socketServer->close_socket();
    delete socketServer;
}
