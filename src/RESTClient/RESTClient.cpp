//
// Created by gergocs on 2023.05.13..
//

#include <HTTPClient.h>
#include "RESTClient.h"
#include "BurstManager.h"

void RESTClient::init(const std::string &ssid, const std::string &password) {
    WiFi.begin(ssid.c_str(), password.c_str());

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
}

DynamicJsonDocument
RESTClient::Get(const std::string &path, const std::string &token, const std::map<std::string, std::string> &params) {
    HTTPClient http;

    if (!params.empty()) {
        std::string paramStr = "?";

        for (const auto& i : params) {
            paramStr += (i.first + "=" + i.second + "&");
        }

        paramStr.erase(paramStr.end() - 1);
        http.begin((path + paramStr).c_str());
    } else {
        http.begin(path.c_str());
    }

    http.addHeader("Content-Type", "application/json");
    http.addHeader("Accept", "application/json");

    if (!token.empty()) {
        http.addHeader("Authorization", ("Bearer " + token).c_str());
    }

    ArduinoJson::DynamicJsonDocument retVal(1024);

    delay(BurstManager::getDelay().count());
    BurstManager::markRequestPosted();

    if (http.GET() > 0) {
        ArduinoJson::deserializeJson(retVal, http.getString().c_str());
    }

    http.end();

    return retVal;
}

DynamicJsonDocument
RESTClient::Post(const std::string &path, const std::string &token, const DynamicJsonDocument &data) {
    HTTPClient http;

    http.begin(path.c_str());

    http.addHeader("Content-Type", "application/json");
    http.addHeader("Accept", "application/json");

    if (!token.empty()) {
        http.addHeader("Authorization", ("Bearer " + token).c_str());
    }

    std::string serializedJson;
    ArduinoJson::serializeJson(data, serializedJson);
    ArduinoJson::DynamicJsonDocument retVal(1024);

    delay(BurstManager::getDelay().count());
    BurstManager::markRequestPosted();

    if (http.POST(serializedJson.c_str()) > 0) {
        ArduinoJson::deserializeJson(retVal, http.getString().c_str());
    }


    return retVal;
}
