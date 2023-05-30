//
// Created by gergocs on 2023.05.19..
//

#include "MenuHandler.h"
#include "RESTClient/RESTClient.h"

__attribute__((unused)) MenuHandler::MenuHandler(int8_t sda, int8_t sdl) : display(Adafruit_SH1106(sda, sdl)),
                                                                           position(0),
                                                                           pageCounter(1), maxPage(1),
                                                                           jsonStorage(1024) {
    this->display.begin(SH1106_SWITCHCAPVCC, 0x3C);
    this->display.clearDisplay();
    this->display.setTextSize(1);
    this->display.setTextColor(WHITE);

    std::vector<String> data = {
            "[*]Agent",
            "[ ]Factions",
            "[ ]Fleet",
            "[ ]Contract",
            "[ ]Systems",
    };

    this->mainMenu = new Menu(data);
    this->currentMenu = mainMenu;
}

MenuHandler::MenuHandler(Adafruit_SH1106 &display) : display(display), position(0), pageCounter(1),
                                                     jsonStorage(1024), maxPage(1) {
    this->display.begin(SH1106_SWITCHCAPVCC, 0x3C);
    this->display.clearDisplay();
    this->display.setTextSize(1);
    this->display.setTextColor(WHITE);

    std::vector<String> data = {
            "[*]Agent",
            "[ ]Factions",
            "[ ]Fleet",
            "[ ]Contract",
            "[ ]Systems",
    };

    this->mainMenu = new Menu(data);
    this->currentMenu = mainMenu;
}

void MenuHandler::renderMenu() {
    this->display.clearDisplay();
    this->display.setCursor(0, 0);

    for (uint16_t i = this->position <= 8 ? 0 : this->position;
         i < std::min(uint32_t(this->position + 8), this->displayData.size()); ++i) {
        this->display.println(this->displayData[i]);
    }

    this->display.display();
}

void MenuHandler::setDisplayData(std::vector<String> &data) {
    this->displayData = data;
    position = 0;
}

void MenuHandler::setDisplayDataMainMenu() {
    this->setDisplayData(const_cast<std::vector<String> &>(this->mainMenu->getMenuItems()));
    this->currentMenu = this->mainMenu;
}

void MenuHandler::prevMenu() {
    if (this->currentMenu->getParent() != nullptr && this->extraInfoCounter == 0) {
        this->currentMenu->clearItems();
        this->currentMenu = this->currentMenu->getParent();
        this->setDisplayData(const_cast<std::vector<String> &>(this->currentMenu->getMenuItems()));
    }

    this->extraInfoCounter = 0;
    this->renderMenu();
}

void MenuHandler::select(const String &token) {
    if (this->extraInfoCounter != 0) {
        return;
    }

    switch (this->currentMenu->getName()) {
        case Menu::MainMenu: {
            switch (this->position) {
                case 0: { // Agent data
                    this->jsonStorage = RESTClient::Get("https://api.spacetraders.io/v2/my/agent",
                                                        token.c_str())["data"];

                    std::vector<String> data = {};

                    data.push_back("id:" + this->jsonStorage["accountId"].as<String>());
                    data.push_back("name:" + this->jsonStorage["symbol"].as<String>());
                    data.push_back("hq:" + this->jsonStorage["headquarters"].as<String>());
                    data.push_back("credits:" + this->jsonStorage["credits"].as<String>());

                    auto tmp = new Menu(data, Menu::AgentMenu, this->currentMenu, false);

                    this->currentMenu->addChild(tmp);
                    this->currentMenu = tmp;

                    break;
                }
                case 1: { // Factions
                    this->jsonStorage = RESTClient::Get("https://api.spacetraders.io/v2/factions",
                                                        token.c_str(), {{"limit", "7"},
                                                                        {"page",  std::to_string(pageCounter)}});

                    std::vector<String> data = {};
                    uint32_t currentPage = 1;
                    uint8_t counter = 0;

                    if (this->jsonStorage["meta"].isNull()) {
                        this->maxPage = 1;
                    } else {
                        this->maxPage = this->jsonStorage["meta"]["total"].as<int>();
                        currentPage = this->jsonStorage["meta"]["page"].as<int>();
                    }

                    for (JsonVariant item: this->jsonStorage["data"].as<ArduinoJson::JsonArray>()) {
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

                    data.emplace_back((std::to_string(currentPage) + "/" + std::to_string(this->maxPage)).c_str());

                    auto tmp = new Menu(data, Menu::FactionMenu, this->currentMenu);

                    this->currentMenu->addChild(tmp);
                    this->currentMenu = tmp;

                    break;
                }
                case 2: { // fleet
                    break;
                }
                case 3: { // contracts
                    this->jsonStorage = RESTClient::Get("https://api.spacetraders.io/v2/my/contracts",
                                                        token.c_str(), {{"limit", "7"},
                                                                        {"page",  std::to_string(pageCounter)}});

                    std::vector<String> data = {};
                    this->idStorage.clear();
                    uint32_t currentPage = 1;
                    uint8_t counter = 0;

                    if (this->jsonStorage["meta"].isNull()) {
                        this->maxPage = 1;
                    } else {
                        this->maxPage = this->jsonStorage["meta"]["total"].as<int>();
                        currentPage = this->jsonStorage["meta"]["page"].as<int>();
                    }

                    for (JsonVariant item: this->jsonStorage["data"].as<ArduinoJson::JsonArray>()) {
                        if (counter == 0) {
                            data.push_back("[*]" + item["id"].as<String>().substring(0, 15) + "...");
                        } else {
                            data.push_back("[ ]" + item["id"].as<String>().substring(0, 15) + "...");
                        }

                        this->idStorage.push_back(item["id"].as<std::string>());

                        counter++;

                        if (counter >= 7) {
                            break;
                        }
                    }

                    for (; counter < 7; ++counter) {
                        data.emplace_back();
                    }

                    data.emplace_back((std::to_string(currentPage) + "/" + std::to_string(this->maxPage)).c_str());

                    auto tmp = new Menu(data, Menu::ContractMenu, this->currentMenu);

                    this->currentMenu->addChild(tmp);
                    this->currentMenu = tmp;

                    break;
                }
                default: {
                    break;
                }

            }
            break;
        }
        case Menu::AgentMenu: {
            break;
        }
        case Menu::FactionMenu: {
            String symbol = this->currentMenu->getMenuItems()[this->position].substring(3);

            this->jsonStorage = RESTClient::Get(("https://api.spacetraders.io/v2/factions/" + symbol).c_str(),
                                                token.c_str(), {{"factionSymbol", symbol.c_str()}});

            std::vector<String> data = {};

            if (this->jsonStorage["data"]["name"].isNull()) {
                break;
            }

            data.emplace_back("name:" + this->jsonStorage["data"]["name"].as<String>());
            data.emplace_back("symbol:" + this->jsonStorage["data"]["symbol"].as<String>());
            data.emplace_back("hq:" + this->jsonStorage["data"]["headquarters"].as<String>());
            data.emplace_back(this->jsonStorage["data"]["isRecruiting"].as<bool>() ? "recruiting" : "not recruiting");

            auto tmp = new Menu(data, Menu::FactionMenu, this->currentMenu, false, this->jsonStorage["data"]["description"].as<String>());

            this->currentMenu->addChild(tmp);
            this->currentMenu = tmp;

            break;
        }
        case Menu::FleetMenu: {
            break;
        }
        case Menu::ContractMenu: {
            std::string symbol = this->idStorage[this->position];

            this->jsonStorage = RESTClient::Get("https://api.spacetraders.io/v2/my/contracts/" + symbol,
                                                token.c_str(), {{"contractId", symbol}});

            ArduinoJson::serializeJson(this->jsonStorage, Serial);

            std::vector<String> data = {};

            if (!this->currentMenu->getMenuItems()[0].startsWith("deadline")) {
                data.emplace_back("deadline: " + this->jsonStorage["data"]["terms"]["deadline"].as<String>());
                data.emplace_back(
                        "1st payment: " + this->jsonStorage["data"]["terms"]["payment"]["onAccepted"].as<String>());
                data.emplace_back(
                        "2nd payment: " + this->jsonStorage["data"]["terms"]["payment"]["onFulfilled"].as<String>());
            } else if (!this->currentMenu->getMenuItems()[0].startsWith("deadline")) {

            } else {
                data.emplace_back(this->jsonStorage["data"]["id"].as<String>().substring(0, 18) + "...");
                data.emplace_back(this->jsonStorage["data"]["factionSymbol"].as<String>());
                data.emplace_back(this->jsonStorage["data"]["type"].as<String>());
                data.emplace_back(this->jsonStorage["data"]["accepted"].as<bool>() ? "accepted" : "not accepted");
                data.emplace_back(this->jsonStorage["data"]["fulfilled"].as<bool>() ? "fulfilled" : "not fulfilled");
                data.emplace_back(this->jsonStorage["data"]["deadlineToAccept"].as<String>());
            }

            auto tmp = new Menu(data, Menu::ContractMenu, this->currentMenu, false);

            this->currentMenu->addChild(tmp);
            this->currentMenu = tmp;

            break;
        }
        case Menu::ContractSubMenu: {
            std::string symbol = this->idStorage[this->position];

            this->jsonStorage = RESTClient::Get("https://api.spacetraders.io/v2/my/contracts/" + symbol,
                                                token.c_str(), {{"contractId", symbol}});

            ArduinoJson::serializeJson(this->jsonStorage, Serial);

            std::vector<String> data = {};


            auto tmp = new Menu(data, Menu::ContractSubMenu, this->currentMenu, false);

            this->currentMenu->addChild(tmp);
            this->currentMenu = tmp;

            break;
        }
        default: {

        }
    }

    this->setDisplayData(const_cast<std::vector<String> &>(this->currentMenu->getMenuItems()));

    this->renderMenu();
}

void MenuHandler::renderSpecial(bool first) {
    if (this->currentMenu->getExtraInfo().isEmpty()) {
        return;
    }

    if (first) {
        this->extraInfoCounter = 1;
    }

    this->display.clearDisplay();
    this->display.setCursor(0, 0);

    this->display.println(this->currentMenu->getExtraInfo().substring((this->extraInfoCounter - 1) * 21, std::min(
            static_cast<unsigned int>(this->extraInfoCounter * 21 * 7), this->currentMenu->getExtraInfo().length())));

    this->display.display();
}

