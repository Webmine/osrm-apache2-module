#ifndef OSRM_CONFIG_HELPER_H_INCLUDED
#define OSRM_CONFIG_HELPER_H_INCLUDED

#include "logger/log.h"
#include <unordered_map>
#include "boost/filesystem/path.hpp"
#include "boost/filesystem/operations.hpp"

inline int populate_server_paths(std::string base_path, std::unordered_map<std::string,boost::filesystem::path>& server_paths)
{
    server_paths["hsgrdata"] = base_path + ".hsgr";
    server_paths["nodesdata"] = base_path + ".nodes";
    server_paths["coredata"] = base_path + ".core";
    server_paths["edgesdata"] = base_path + ".edges";
    server_paths["geometries"] = base_path + ".geometry";
    server_paths["ramindex"] = base_path + ".ramIndex";
    server_paths["fileindex"] = base_path + ".fileIndex";
    server_paths["namesdata"] = base_path + ".names";
    server_paths["timestamp"] = base_path + ".timestamp";

    bool hasErrors = false;
    for(auto it = server_paths.begin(); it != server_paths.end(); ++it)
    {
        if(!boost::filesystem::is_regular_file(boost::filesystem::canonical(it->second)))
        {
            LOG_ERROR("File not exists %s", it->second.string().c_str());
            hasErrors = true;
        }
    }
    if(hasErrors)
        return 1;
    else
        return 0;
}

#endif // OSRM_CONFIG_HELPER_H_INCLUDED
