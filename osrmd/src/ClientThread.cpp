#include "ClientThread.h"
#include "osrm_config_helper.h"
#include "osrm/osrm.hpp"
#include "osrm/route_parameters.hpp"
#include "osrm/util/coordinate.hpp"
#include "osrm/json_container.hpp"
#include "json.h"
#include <iostream>

using namespace osrm;
using namespace osrm::engine;
using JSON = nlohmann::json;

ClientThread::ClientThread(Server* server, int number, EngineConfig& config)
{
    this->server = server;
    this->threadNum = number;
    this->osrmEngine = new OSRM(config);
}

ClientThread::~ClientThread()
{
}

void ClientThread::Prepare(int& newClient)
{
    client = newClient;
}

void ClientThread::Reset()
{
    client = 0;
}

void ClientThread::InternalThreadEntry()
{

        // get a request
    string request = server->get_request(client);

    if (!request.empty())
    {
        //OSRM ENGINE

        RouteParameters route_parameters;
        route_parameters.geometry = true;
        route_parameters.service = "viaroute";
        route_parameters.output_format = "json";

        route_parameters.coordinates.emplace_back(52.519930 * COORDINATE_PRECISION, 13.438640 * COORDINATE_PRECISION);
        route_parameters.coordinates.emplace_back(52.513191 * COORDINATE_PRECISION, 13.415852 * COORDINATE_PRECISION);

        osrm::json::Object json_result;
        const int result_code = osrmEngine->RunQuery(route_parameters, json_result);
        auto asd = json_result.values;

        std::cout << typeid(asd).name();

        //JSON j_umap();

        //EOF OSRM ENGINE

        // send response
        //bool success = server->send_response(client,j_umap);
    }

    close(client);
    server->OnThreadFinished((BaseThread*)this);
}

void ClientThread::RunOsrmQuery()
{
}

