#include "socket/UnixServer.h"
#include "ini-reader/INIReader.h"
#include "logger/log.h"

int main(int argc, char **argv)
{
    /*log_init(LL_TRACE, "osrmd", "./log");
    LOG_NOTICE("%s [time:%d]", "test calling log", time(NULL));
    LOG_DEBUG("debug msg,only write to log when the loglevel bigger than or equal to debug [time:%d]", time(NULL));
    LOG_WARN("warnning msg will be writing to the error files [time:%d]", time(NULL));
    LOG_ERROR("you also can change  number of output files by rewrite the macro_define.h");*/

    UnixServer server = UnixServer();
    server.run();
}
