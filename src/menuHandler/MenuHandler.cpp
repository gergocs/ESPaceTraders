//
// Created by gergocs on 2023.05.19..
//

#include "MenuHandler.h"
#include "RESTClient/RESTClient.h"

__attribute__((unused)) MenuHandler::MenuHandler(int8_t sda, int8_t sdl) : display(Adafruit_SH1106(sda, sdl)),
                                                                           position(0), needStar(true),
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

MenuHandler::MenuHandler(Adafruit_SH1106 &display) : display(display), position(0), needStar(true), pageCounter(1),
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

void MenuHandler::setDisplayData(std::vector<String> &data, bool star) {
    this->displayData = data;
    position = 0;
    this->needStar = star;
}

void MenuHandler::setDisplayDataMainMenu() {
    this->needStar = true;

    this->setDisplayData(const_cast<std::vector<String> &>(this->mainMenu->getMenuItems()));
    this->currentMenu = this->mainMenu;
}

void MenuHandler::prevMenu() {
    if (this->currentMenu->getParent() != nullptr) {
        this->currentMenu->clearItems();
        this->currentMenu = this->currentMenu->getParent();
        this->setDisplayData(const_cast<std::vector<String> &>(this->currentMenu->getMenuItems()));
    }
}

void MenuHandler::select(const String &token) {
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

                    auto tmp = new Menu(data, Menu::AgentMenu, this->currentMenu);

                    this->currentMenu->addChild(tmp);
                    this->currentMenu = tmp;
                    this->needStar = false;

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
                    this->needStar = true;

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

            data.emplace_back("name:" + this->jsonStorage["data"]["name"].as<String>());
            data.emplace_back("symbol:" + this->jsonStorage["data"]["symbol"].as<String>());
            data.emplace_back("hq:" + this->jsonStorage["data"]["headquarters"].as<String>());
            /*data.emplace_back(jsonData["data"]["description"].as<String>());*/ //TODO
            data.emplace_back(this->jsonStorage["data"]["isRecruiting"].as<bool>() ? "recruiting" : "not recruiting");

            auto tmp = new Menu(data, Menu::FactionMenu, this->currentMenu);

            this->currentMenu->addChild(tmp);
            this->currentMenu = tmp;
            this->needStar = false;

            break;
        }
        case Menu::FleetMenu: {
            break;
        }
        default: {

        }
    }

    this->setDisplayData(const_cast<std::vector<String> &>(this->currentMenu->getMenuItems()));

}

