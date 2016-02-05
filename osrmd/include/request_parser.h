#ifndef REQUEST_PARSER_H_INCLUDED
#define REQUEST_PARSER_H_INCLUDED

#include "osrm/route_parameters.hpp"
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>

class RequestParser
{

public:
    static RouteParameters parseRequest(std::string request)
    {
        int questionMarkPos = request.find('?');
        auto action = request.substr(0, questionMarkPos);
        auto paramsString = request.substr(questionMarkPos + 1, request.length() - (questionMarkPos + 1 ));

        std::vector<std::string> params;
        boost::split(params, paramsString, boost::is_any_of("&"));

        RouteParameters route_parameters;
        route_parameters.service = action;

        for(std::vector<std::string>::iterator it = params.begin(); it != params.end(); it++)
        {
            std::vector<std::string> paramValues;
            boost::split(paramValues, *it, boost::is_any_of("=,"));

            if(paramValues[0] == "loc" && paramValues.size() > 2)
            {
                route_parameters.coordinates.emplace_back(atof(paramValues[1].c_str()) * COORDINATE_PRECISION, atof(paramValues[2].c_str()) * COORDINATE_PRECISION);
            }
        }

        return route_parameters;
    }
};

#endif // REQUEST_PARSER_H_INCLUDED
