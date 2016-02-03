#include "ClientThread.h"
#include "osrm_config_helper.h"
#include "osrm/osrm.hpp"
#include "osrm/route_parameters.hpp"
#include "util/coordinate.hpp"
#include "osrm/json_container.hpp"
#include "util/json_renderer.hpp"
#include <iostream>

using namespace osrm;
using namespace osrm::engine;

ClientThread::ClientThread(Server* server, int number, EngineConfig& config)
{
    this->server = server;
    this->threadNum = number;
    this->engineConfig = config;
}

ClientThread::~ClientThread()
{
}

void ClientThread::Prepare(int& newClient)
{
    osrmEngine = new OSRM(engineConfig);
    client = newClient;
}

void ClientThread::Reset()
{
    client = 0;
    delete osrmEngine;
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

        route_parameters.coordinates.emplace_back(47.49744 * COORDINATE_PRECISION, 19.03584 * COORDINATE_PRECISION);
        route_parameters.coordinates.emplace_back(47.50479 * COORDINATE_PRECISION, 19.14778 * COORDINATE_PRECISION);

        osrm::json::Object json_result;
        const int result_code = osrmEngine->RunQuery(route_parameters, json_result);

        std::vector<char> response;
        osrm::json::render(response, json_result);

        // send response
        server->send_response(client,&*response.begin());
    }

    close(client);
    server->OnThreadFinished((BaseThread*)this);
}

void ClientThread::RunOsrmQuery()
{
}

