#include "http_parser.h"

#include<sstream>   

bool HttpParser::parse(const std::string& raw, HttpRequest& request){
    std::istringstream stream(raw);
    std::string line;

    if(!std::getline(stream, line))
        return false;

    if(!line.empty() && line.back() == '\r')
        line.pop_back();

    std::istringstream request_line(line);
    request_line >> request.method >> request.path >> request.version;

    if(request.method.empty() || request.path.empty()){
        return false;
    }

    while(std::getline(stream, line)){
        if(line == "\r" || line == "") break;

        if(!line.empty() && line.back() == '\r')
            line.pop_back();

        size_t colon = line.find(':');
        if(colon == std::string::npos)
            continue;

        std::string key = line.substr(0, colon);
        std::string value = line.substr(colon + 1);

        if(!value.empty() && value[0] == ' ')
            value.erase(0, 1);
        
        request.headers[key] = value;

        if(key == "Host")
            request.host = value;
    }

    return true;
}

