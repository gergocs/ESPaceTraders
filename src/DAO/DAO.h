//
// Created by gergocs on 2023.05.31..
//

#ifndef ESPACETRADERS_DAO_H
#define ESPACETRADERS_DAO_H


#include <ArduinoJson.h>

class DAO {
public:
    static std::vector<String> getAgentInfo(ArduinoJson::DynamicJsonDocument& jsonStorage, const String &token);
    static std::vector<String> getFactions(ArduinoJson::DynamicJsonDocument& jsonStorage, const String &token, uint32_t &maxPage,uint32_t pageCounter);
    static std::vector<String> getFaction(ArduinoJson::DynamicJsonDocument& jsonStorage, const String &token, const String& symbol);
    static std::vector<String> getContracts(ArduinoJson::DynamicJsonDocument& jsonStorage, const String &token, uint32_t &maxPage,uint32_t pageCounter, std::vector<std::string>& idStorage);
    static std::vector<String> getContract(ArduinoJson::DynamicJsonDocument& jsonStorage, const String &token, const std::string& symbol, String& items);
    static std::vector<String> getContractSubMenu(ArduinoJson::DynamicJsonDocument& jsonStorage);
};


#endif //ESPACETRADERS_DAO_H
