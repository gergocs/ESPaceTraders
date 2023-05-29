//
// Created by gergocs on 2023.05.13..
//

#ifndef ESPACETRADERS_RESTCLIENT_H
#define ESPACETRADERS_RESTCLIENT_H


#include <ArduinoJson.h>

#include <string>
#include <map>

class RESTClient {
public:
    static void init(const std::string &ssid, const std::string &password);
    static DynamicJsonDocument Get(const std::string& path, const std::string& token, const std::map<std::string, std::string>& params = {});
    static DynamicJsonDocument Post(const std::string& path, const std::string& token, const DynamicJsonDocument& data);
};


#endif //ESPACETRADERS_RESTCLIENT_H
