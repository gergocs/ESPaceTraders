//
// Created by gergocs on 2023.05.31..
//

#include "DAO.h"
#include "RESTClient/RESTClient.h"

std::vector<String> DAO::getAgentInfo(ArduinoJson::DynamicJsonDocument &jsonStorage, const String &token) {
    jsonStorage = RESTClient::Get("https://api.spacetraders.io/v2/my/agent", token.c_str())["data"];

    std::vector<String> data = {};

    data.push_back("id:" + jsonStorage["accountId"].as<String>());
    data.push_back("name:" + jsonStorage["symbol"].as<String>());
    data.push_back("hq:" + jsonStorage["headquarters"].as<String>());
    data.push_back("credits:" + jsonStorage["credits"].as<String>());

    return data;
}

std::vector<String> DAO::getFactions(ArduinoJson::DynamicJsonDocument &jsonStorage, const String &token, uint32_t &maxPage, uint32_t pageCounter) {
    jsonStorage = RESTClient::Get("https://api.spacetraders.io/v2/factions", token.c_str(), {{"limit", "7"},
                                                                                             {"page",  std::to_string(pageCounter)}});

    std::vector<String> data = {};
    uint32_t currentPage = 1;
    uint8_t counter = 0;

    if (jsonStorage["meta"].isNull()) {
        maxPage = 1;
    } else {
        maxPage = jsonStorage["meta"]["total"].as<int>();
        currentPage = jsonStorage["meta"]["page"].as<int>();
    }

    for (JsonVariant item: jsonStorage["data"].as<ArduinoJson::JsonArray>()) {
        if (counter == 0) {
            data.push_back("[*]" + item["symbol"].as<String>());
        } else {
            data.push_back("[ ]" + item["symbol"].as<String>());
        }

        counter++;

        if (counter >= 7) {
            break;
        }
    }

    for (; counter < 7; ++counter) {
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

    if (jsonStorage["meta"].isNull()) {
        maxPage = 1;
    } else {
        maxPage = jsonStorage["meta"]["total"].as<int>();
        currentPage = jsonStorage["meta"]["page"].as<int>();
    }

    for (JsonVariant item: jsonStorage["data"].as<ArduinoJson::JsonArray>()) {
        if (counter == 0) {
            data.push_back("[*]" + item["id"].as<String>().substring(0, 15) + "...");
        } else {
            data.push_back("[ ]" + item["id"].as<String>().substring(0, 15) + "...");
        }

        idStorage.push_back(item["id"].as<std::string>());

        counter++;

        if (counter >= 7) {
            break;
        }
    }

    for (; counter < 7; ++counter) {
        data.emplace_back();
    }

    data.emplace_back((std::to_string(currentPage) + "/" + std::to_string(maxPage)).c_str());

    return data;
}

std::vector<String> DAO::getFaction(DynamicJsonDocument &jsonStorage, const String &token, const String &symbol) {
    jsonStorage = RESTClient::Get(("https://api.spacetraders.io/v2/factions/" + symbol).c_str(),
                                  token.c_str(), {{"factionSymbol", symbol.c_str()}});

    std::vector<String> data = {};

    if (jsonStorage["data"]["name"].isNull()) {
        return {};
    }

    data.emplace_back("name:" + jsonStorage["data"]["name"].as<String>());
    data.emplace_back("symbol:" + jsonStorage["data"]["symbol"].as<String>());
    data.emplace_back("hq:" + jsonStorage["data"]["headquarters"].as<String>());
    data.emplace_back(jsonStorage["data"]["isRecruiting"].as<bool>() ? "recruiting" : "not recruiting");

    return data;
}

std::vector<String> DAO::getContract(DynamicJsonDocument &jsonStorage, const String &token, const std::string &symbol, String &items) {
    jsonStorage = RESTClient::Get("https://api.spacetraders.io/v2/my/contracts/" + symbol,
                                  token.c_str(), {{"contractId", symbol}});

    std::vector<String> data = {};

    data.emplace_back("deadline: " + jsonStorage["data"]["terms"]["deadline"].as<String>());
    data.emplace_back("1st payment: " + jsonStorage["data"]["terms"]["payment"]["onAccepted"].as<String>());
    data.emplace_back("2nd payment: " + jsonStorage["data"]["terms"]["payment"]["onFulfilled"].as<String>());
    data.emplace_back("dst: " + jsonStorage["data"]["terms"]["deliver"].as<ArduinoJson::JsonArray>()[0]["destinationSymbol"].as<String>()); // TODO maybe change this

    for (JsonVariant item: jsonStorage["data"]["terms"]["deliver"].as<ArduinoJson::JsonArray>()) { // TODO order by destination
        if (uint32_t fulfilled = item["unitsFulfilled"].as<uint32_t>(), required = item["unitsRequired"].as<uint32_t>(); fulfilled < required) {
            items += item["tradeSymbol"].as<String>() + " " + String(required - fulfilled) + "\n"; // TODO number float to right
        }
    }

    return data;
}

std::vector<String> DAO::getContractSubMenu(DynamicJsonDocument &jsonStorage) {
    std::vector<String> data = {};

    data.emplace_back(jsonStorage["data"]["id"].as<String>().substring(0, 18) + "...");
    data.emplace_back(jsonStorage["data"]["factionSymbol"].as<String>());
    data.emplace_back(jsonStorage["data"]["type"].as<String>());
    data.emplace_back(jsonStorage["data"]["accepted"].as<bool>() ? "accepted" : "not accepted");
    data.emplace_back(jsonStorage["data"]["fulfilled"].as<bool>() ? "fulfilled" : "not fulfilled");
    data.emplace_back("deadLine:" + jsonStorage["data"]["deadlineToAccept"].as<String>());

    return data;
}
