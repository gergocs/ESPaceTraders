//
// Created by gergocs on 2023.05.19..
//

#ifndef ESPACETRADERS_MENUHANDLER_H
#define ESPACETRADERS_MENUHANDLER_H

#include <Arduino.h>
#include <vector>
#include "Adafruit_SH1106.h"
#include "Menu.h"
#include <ArduinoJson.h>

class MenuHandler {
public:
    __attribute__((unused)) MenuHandler(int8_t sda, int8_t sdl);

    explicit MenuHandler(Adafruit_SH1106 &display);

    void renderMenu();

    void forWard() {
        if (this->extraInfoCounter != 0) {
            if (this->extraInfoCounter <= (this->currentMenu->getExtraInfo().length() - (7 * 21)) / 21) {
                this->extraInfoCounter++;
            }

            this->renderSpecial(false);

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

    void backWard() {
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

    void renderSpecial(bool first = true);

    void prevMenu();

    void setDisplayData(std::vector<String> &data);

    void setDisplayDataMainMenu();

    void select(const String &token);

private:
    ArduinoJson::DynamicJsonDocument jsonStorage;
    std::vector<String> displayData;
    std::vector<std::string> idStorage;
    Menu *mainMenu;
    Menu *currentMenu;
    Adafruit_SH1106 display;
    uint16_t position;
    uint32_t pageCounter;
    uint32_t maxPage;
    uint8_t extraInfoCounter = 0;
};


#endif //ESPACETRADERS_MENUHANDLER_H
