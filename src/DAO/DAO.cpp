//
// Created by gergocs on 2023.05.31..
//

#include "DAO.h"
#include "RESTClient/RESTClient.h"
#include "menuHandler/MenuHandler.h"
#include "utils/JSONEnum.h"

std::vector<String> DAO::getAgentInfo(ArduinoJson::DynamicJsonDocument &jsonStorage, const String &token) {
    jsonStorage = RESTClient::Get("https://api.spacetraders.io/v2/my/agent", token.c_str())[getEnumAsString(JSONEnum::DATA)];

    std::vector<String> data = {};

    data.push_back("id:" + jsonStorage[getEnumAsString(JSONEnum::ACCOUNT_ID)].as<String>());
    data.push_back("name:" + jsonStorage[getEnumAsString(JSONEnum::SYMBOL)].as<String>());
    data.push_back("hq:" + jsonStorage[getEnumAsString(JSONEnum::HEADQUARTERS)].as<String>());
    data.push_back("credits:" + jsonStorage[getEnumAsString(JSONEnum::CREDITS)].as<String>());

    return data;
}

std::vector<String> DAO::getFactions(ArduinoJson::DynamicJsonDocument &jsonStorage, const String &token, uint32_t &maxPage, uint32_t pageCounter) {
    jsonStorage = RESTClient::Get("https://api.spacetraders.io/v2/factions", token.c_str(), {{"limit", "7"},
                                                                                             {"page",  std::to_string(pageCounter)}});

    std::vector<String> data = {};
    uint32_t currentPage = 1;
    uint8_t counter = 0;

    if (jsonStorage[getEnumAsString(JSONEnum::META)].isNull()) {
        maxPage = 1;
    } else {
        maxPage = jsonStorage[getEnumAsString(JSONEnum::META)][getEnumAsString(JSONEnum::TOTAL)].as<int>();
        currentPage = jsonStorage[getEnumAsString(JSONEnum::META)][getEnumAsString(JSONEnum::PAGE)].as<int>();
    }

    for (JsonVariant item: jsonStorage[getEnumAsString(JSONEnum::DATA)].as<ArduinoJson::JsonArray>()) {
        if (counter == 0) {
            data.push_back("[*]" + item[getEnumAsString(JSONEnum::SYMBOL)].as<String>());
        } else {
            data.push_back("[ ]" + item[getEnumAsString(JSONEnum::SYMBOL)].as<String>());
        }

        counter++;

        if (counter >= 7) {
            break;
        }
    }

    for (; counter < NUMBER_OF_LINES - 1; ++counter) {
        data.emplace_back();
    }

    data.emplace_back((std::to_string(currentPage) + "/" + std::to_string(maxPage)).c_str());

    return data;
}

std::vector<String> DAO::getContracts(ArduinoJson::DynamicJsonDocument &jsonStorage, const String &token, uint32_t &maxPage, uint32_t pageCounter, std::vector<std::string> &idStorage) {
    jsonStorage = RESTClient::Get("https://api.spacetraders.io/v2/my/contracts", token.c_str(), {{"limit", "7"},
                                                                                                 {"page",  std::to_string(pageCounter)}});

    std::vector<String> data = {};
    idStorage.clear();
    uint32_t currentPage = 1;
    uint8_t counter = 0;

    if (jsonStorage[getEnumAsString(JSONEnum::META)].isNull()) {
        maxPage = 1;
    } else {
        maxPage = jsonStorage[getEnumAsString(JSONEnum::META)][getEnumAsString(JSONEnum::TOTAL)].as<int>();
        currentPage = jsonStorage[getEnumAsString(JSONEnum::META)][getEnumAsString(JSONEnum::PAGE)].as<int>();
    }

    for (JsonVariant item: jsonStorage[getEnumAsString(JSONEnum::DATA)].as<ArduinoJson::JsonArray>()) {
        if (counter == 0) {
            data.push_back("[*]" + item[getEnumAsString(JSONEnum::ID)].as<String>().substring(0, 15) + "...");
        } else {
            data.push_back("[ ]" + item[getEnumAsString(JSONEnum::ID)].as<String>().substring(0, 15) + "...");
        }

        idStorage.push_back(item[getEnumAsString(JSONEnum::ID)].as<std::string>());

        counter++;

        if (counter >= NUMBER_OF_LINES - 1) {
            break;
        }
    }

    for (; counter < NUMBER_OF_LINES - 1; ++counter) {
        data.emplace_back();
    }

    data.emplace_back((std::to_string(currentPage) + "/" + std::to_string(maxPage)).c_str());

    return data;
}

std::vector<String> DAO::getFaction(DynamicJsonDocument &jsonStorage, const String &token, const String &symbol) {
    jsonStorage = RESTClient::Get(("https://api.spacetraders.io/v2/factions/" + symbol).c_str(),
                                  token.c_str(), {{"factionSymbol", symbol.c_str()}});

    std::vector<String> data = {};

    if (jsonStorage[getEnumAsString(JSONEnum::DATA)][getEnumAsString(JSONEnum::NAME)].isNull()) {
        return {};
    }

    data.emplace_back("name:" + jsonStorage[getEnumAsString(JSONEnum::DATA)][getEnumAsString(JSONEnum::NAME)].as<String>());
    data.emplace_back("symbol:" + jsonStorage[getEnumAsString(JSONEnum::DATA)][getEnumAsString(JSONEnum::SYMBOL)].as<String>());
    data.emplace_back("hq:" + jsonStorage[getEnumAsString(JSONEnum::DATA)][getEnumAsString(JSONEnum::HEADQUARTERS)].as<String>());
    data.emplace_back(jsonStorage[getEnumAsString(JSONEnum::DATA)][getEnumAsString(JSONEnum::IS_RECRUITING)].as<bool>() ? "recruiting" : "not recruiting");

    return data;
}

std::vector<String> DAO::getContract(DynamicJsonDocument &jsonStorage, const String &token, const std::string &symbol, String &items) {
    jsonStorage = RESTClient::Get("https://api.spacetraders.io/v2/my/contracts/" + symbol,
                                  token.c_str(), {{"contractId", symbol}});

    std::vector<String> data = {};

    data.emplace_back("deadline: " + jsonStorage[getEnumAsString(JSONEnum::DATA)][getEnumAsString(JSONEnum::TERMS)][getEnumAsString(JSONEnum::DEADLINE)].as<String>());
    data.emplace_back("1st payment: " + jsonStorage[getEnumAsString(JSONEnum::DATA)][getEnumAsString(JSONEnum::TERMS)][getEnumAsString(JSONEnum::PAYMENT)][getEnumAsString(JSONEnum::ON_ACCEPTED)].as<String>());
    data.emplace_back("2nd payment: " + jsonStorage[getEnumAsString(JSONEnum::DATA)][getEnumAsString(JSONEnum::TERMS)][getEnumAsString(JSONEnum::PAYMENT)][getEnumAsString(JSONEnum::ON_FULFILLED)].as<String>());
    data.emplace_back("dst: " + jsonStorage[getEnumAsString(JSONEnum::DATA)][getEnumAsString(JSONEnum::TERMS)][getEnumAsString(JSONEnum::DELIVER)].as<ArduinoJson::JsonArray>()[0][getEnumAsString(JSONEnum::DESTINATION_SYMBOL)].as<String>()); // TODO maybe change this

    for (JsonVariant item: jsonStorage[getEnumAsString(JSONEnum::DATA)][getEnumAsString(JSONEnum::TERMS)][getEnumAsString(JSONEnum::DELIVER)].as<ArduinoJson::JsonArray>()) { // TODO order by destination
        if (uint32_t fulfilled = item[getEnumAsString(JSONEnum::UNITS_FULFILLED)].as<uint32_t>(), required = item[getEnumAsString(JSONEnum::UNITS_REQUIRED)].as<uint32_t>(); fulfilled < required) {
            items += item[getEnumAsString(JSONEnum::TRADE_SYMBOL)].as<String>() + " " + String(required - fulfilled) + "\n"; // TODO number float to right
        }
    }

    return data;
}

std::vector<String> DAO::getContractSubMenu(DynamicJsonDocument &jsonStorage) {
    std::vector<String> data = {};

    data.emplace_back(jsonStorage[getEnumAsString(JSONEnum::DATA)][getEnumAsString(JSONEnum::ID)].as<String>().substring(0, 18) + "...");
    data.emplace_back(jsonStorage[getEnumAsString(JSONEnum::DATA)][getEnumAsString(JSONEnum::FACTION_SYMBOL)].as<String>());
    data.emplace_back(jsonStorage[getEnumAsString(JSONEnum::DATA)][getEnumAsString(JSONEnum::TYPE)].as<String>());
    data.emplace_back(jsonStorage[getEnumAsString(JSONEnum::DATA)][getEnumAsString(JSONEnum::ACCEPTED)].as<bool>() ? "accepted" : "not accepted");
    data.emplace_back(jsonStorage[getEnumAsString(JSONEnum::DATA)][getEnumAsString(JSONEnum::FULFILLED)].as<bool>() ? "fulfilled" : "not fulfilled");
    data.emplace_back("deadLine:" + jsonStorage[getEnumAsString(JSONEnum::DATA)][getEnumAsString(JSONEnum::DEADLINE_TO_ACCEPT)].as<String>());

    return data;
}

bool DAO::acceptContract(DynamicJsonDocument &jsonStorage, const String &token, const std::string &id) {
    jsonStorage.clear();
    jsonStorage[getEnumAsString(JSONEnum::CONTRACT_ID)] = id;

    jsonStorage = RESTClient::Post("https://api.spacetraders.io/v2/my/contracts/" + id + "/accept",
                                   token.c_str(), jsonStorage);

    ArduinoJson::serializeJson(jsonStorage, Serial);

    return !jsonStorage[getEnumAsString(JSONEnum::DATA)][getEnumAsString(JSONEnum::CONTRACT)][getEnumAsString(JSONEnum::ACCEPTED)].isNull() && jsonStorage[getEnumAsString(JSONEnum::DATA)][getEnumAsString(JSONEnum::CONTRACT)][getEnumAsString(JSONEnum::ACCEPTED)].as<bool>();
}

bool DAO::fulfillContract(DynamicJsonDocument &jsonStorage, const String &token, const std::string &id) {
    jsonStorage.clear();
    jsonStorage[getEnumAsString(JSONEnum::CONTRACT_ID)] = id;

    jsonStorage = RESTClient::Post("https://api.spacetraders.io/v2/my/contracts/" + id + "/fulfill",
                                   token.c_str(), jsonStorage);


    ArduinoJson::serializeJson(jsonStorage, Serial);

    return !jsonStorage[getEnumAsString(JSONEnum::DATA)][getEnumAsString(JSONEnum::CONTRACT)][getEnumAsString(JSONEnum::FULFILLED)].isNull() && jsonStorage[getEnumAsString(JSONEnum::DATA)][getEnumAsString(JSONEnum::CONTRACT)][getEnumAsString(JSONEnum::FULFILLED)].as<bool>();
}
