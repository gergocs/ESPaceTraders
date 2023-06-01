//
// Created by gergocs on 2023.05.19..
//

#include "MenuHandler.h"
#include "utils/JSONEnum.h"

#include <utility>

__attribute__((unused)) MenuHandler::MenuHandler(int8_t sda, int8_t sdl, String token) : display(Adafruit_SH1106(sda, sdl)),
                                                                                         position(0),
                                                                                         pageCounter(1), maxPage(1), token(std::move(token)),
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

MenuHandler::MenuHandler(Adafruit_SH1106 &display, String token) : display(display), position(0), pageCounter(1),
                                                                   jsonStorage(1024), maxPage(1), token(std::move(token)) {
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

    for (uint16_t i = this->position <= NUMBER_OF_LINES ? 0 : this->position; i < std::min(uint32_t(this->position + NUMBER_OF_LINES), this->displayData.size()); ++i) {
        this->display.println(this->displayData[i]);
    }

    this->display.display();
}

void MenuHandler::forWard() {
    if (this->extraInfoCounter != 0) {
        if (this->extraInfoCounter <= (this->currentMenu->getExtraInfo().length() - (7 * 21)) / 21) {
            this->extraInfoCounter++;
        }

        this->renderSpecial(false);

        return;
    }

    if (this->currentMenu->getName() == Menu::ContractSubMenu) {
        DAO::fulfillContract(this->jsonStorage, this->token, this->jsonStorage[getEnumAsString(JSONEnum::DATA)][getEnumAsString(JSONEnum::ID)].as<std::string>());

        this->prevMenu();

        return;
    }

    if (this->currentMenu->isNeedStar()) {
        this->displayData[position][1] = ' ';
    } else {
        return;
    }

    this->position++;

    if (this->position == this->displayData.size() || this->displayData[position].isEmpty()) {
        this->position = 0;
    }

    if (this->currentMenu->isNeedStar()) {
        this->displayData[position][1] = '*';
    }

    this->renderMenu();
}

void MenuHandler::backWard() {
    if (this->extraInfoCounter != 0) {
        if (this->extraInfoCounter > 1) {
            this->extraInfoCounter--;
        }

        this->renderSpecial(false);

        return;
    }

    if (this->currentMenu->isNeedStar()) {
        this->displayData[position][1] = ' ';
    } else {
        return;
    }

    if (this->position == 0) {
        uint8_t pos = this->displayData.size() - 1;

        for (uint8_t i = this->displayData.size() - 1; i >= 0; --i) {
            if (!this->displayData[i].isEmpty() && this->displayData[i].startsWith("[ ]")) {
                pos = i;
                break;
            }
        }

        this->position = pos;
    } else {
        this->position--;
    }

    if (this->currentMenu->isNeedStar()) {
        this->displayData[position][1] = '*';
    }

    this->renderMenu();
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

void MenuHandler::select() {
    if (this->extraInfoCounter != 0) {
        return;
    }

    switch (this->currentMenu->getName()) {
        case Menu::MainMenu: {
            switch (this->position) {
                case 0: { // Agent data
                    auto tmp = new Menu(DAO::getAgentInfo(this->jsonStorage, this->token), Menu::AgentMenu, this->currentMenu, false);

                    this->currentMenu->addChild(tmp);
                    this->currentMenu = tmp;

                    break;
                }
                case 1: { // Factions
                    auto tmp = new Menu(DAO::getFactions(this->jsonStorage, this->token, this->maxPage, this->pageCounter), Menu::FactionMenu, this->currentMenu);

                    this->currentMenu->addChild(tmp);
                    this->currentMenu = tmp;

                    break;
                }
                case 2: { // fleet
                    break;
                }
                case 3: { // contracts
                    auto tmp = new Menu(DAO::getContracts(this->jsonStorage, this->token, this->maxPage, this->pageCounter, this->idStorage), Menu::ContractMenu, this->currentMenu);

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
            auto data = DAO::getFaction(this->jsonStorage, this->token, this->currentMenu->getMenuItems()[this->position].substring(3));

            if (data.empty()) [[unlikely]] {
                break;
            }

            auto tmp = new Menu(data, Menu::FactionMenu, this->currentMenu, false, this->jsonStorage[getEnumAsString(JSONEnum::DATA)][getEnumAsString(JSONEnum::DESCRIPTION)].as<String>());

            this->currentMenu->addChild(tmp);
            this->currentMenu = tmp;

            break;
        }
        case Menu::FleetMenu: {
            break;
        }
        case Menu::ContractMenu: {
            String items;

            auto tmp = new Menu(DAO::getContract(this->jsonStorage, this->token, this->idStorage[this->position], items), Menu::ContractSubMenu, this->currentMenu, false, items);

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
    if (this->currentMenu->getName() == Menu::ContractMenu) {
        if (DAO::acceptContract(this->jsonStorage, this->token, this->idStorage[this->position])) {
            this->currentMenu->setExtraInfo("Contract accepted");
        } else {
            this->currentMenu->setExtraInfo(this->jsonStorage[getEnumAsString(JSONEnum::ERROR)][getEnumAsString(JSONEnum::MESSAGE)].as<String>());
        }
    }

    if (this->currentMenu->getExtraInfo().isEmpty()) {
        return;
    }

    if (first) {
        this->extraInfoCounter = 1;
    }

    this->display.clearDisplay();
    this->display.setCursor(0, 0);

    this->display.println(this->currentMenu->getExtraInfo().substring((this->extraInfoCounter - 1) * CHAR_PER_LINE, std::min(static_cast<unsigned int>(this->extraInfoCounter * CHAR_PER_LINE * (NUMBER_OF_LINES - 1)), this->currentMenu->getExtraInfo().length())));

    this->display.display();
}
