//
// Created by gergocs on 2023.05.19..
//

#include "MenuHandler.h"
#include "DAO/DAO.h"

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

    for (uint16_t i = this->position <= 8 ? 0 : this->position; i < std::min(uint32_t(this->position + 8), this->displayData.size()); ++i) {
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
    this->renderMenu();
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
                    auto tmp = new Menu(DAO::getAgentInfo(this->jsonStorage, token), Menu::AgentMenu, this->currentMenu, false);

                    this->currentMenu->addChild(tmp);
                    this->currentMenu = tmp;

                    break;
                }
                case 1: { // Factions
                    auto tmp = new Menu(DAO::getFactions(this->jsonStorage, token, this->maxPage, this->pageCounter), Menu::FactionMenu, this->currentMenu);

                    this->currentMenu->addChild(tmp);
                    this->currentMenu = tmp;

                    break;
                }
                case 2: { // fleet
                    break;
                }
                case 3: { // contracts
                    auto tmp = new Menu(DAO::getContracts(this->jsonStorage, token, this->maxPage, this->pageCounter, this->idStorage), Menu::ContractMenu, this->currentMenu);

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
            auto data = DAO::getFaction(this->jsonStorage, token, this->currentMenu->getMenuItems()[this->position].substring(3));

            if (data.empty()) [[unlikely]] {
                break;
            }

            auto tmp = new Menu(data, Menu::FactionMenu, this->currentMenu, false, this->jsonStorage["data"]["description"].as<String>());

            this->currentMenu->addChild(tmp);
            this->currentMenu = tmp;

            break;
        }
        case Menu::FleetMenu: {
            break;
        }
        case Menu::ContractMenu: {
            String items;

            auto tmp = new Menu(DAO::getContract(this->jsonStorage, token, this->idStorage[this->position], items), Menu::ContractSubMenu, this->currentMenu, false, items);

            this->currentMenu->addChild(tmp);
            this->currentMenu = tmp;

            break;
        }
        case Menu::ContractSubMenu: {
            if (this->currentMenu->getParent()->getName() != Menu::ContractMenu) {
                break;
            }

            auto tmp = new Menu(DAO::getContractSubMenu(this->jsonStorage), Menu::ContractSubMenu, this->currentMenu, false);

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

    this->display.println(this->currentMenu->getExtraInfo().substring((this->extraInfoCounter - 1) * 21, std::min(static_cast<unsigned int>(this->extraInfoCounter * 21 * 7), this->currentMenu->getExtraInfo().length())));

    this->display.display();
}

