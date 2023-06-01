//
// Created by gergocs on 2023.05.19..
//

#ifndef ESPACETRADERS_MENUHANDLER_H
#define ESPACETRADERS_MENUHANDLER_H

#define NUMBER_OF_LINES 8
#define CHAR_PER_LINE 21

#include <Arduino.h>
#include <vector>
#include "Adafruit_SH1106.h"
#include "Menu.h"
#include "DAO/DAO.h"
#include <ArduinoJson.h>

class MenuHandler {
public:
    __attribute__((unused)) MenuHandler(int8_t sda, int8_t sdl, String  token);

    explicit MenuHandler(Adafruit_SH1106 &display, String  token);

    void renderMenu();

    void forWard();

    void backWard();

    void renderSpecial(bool first = true);

    void prevMenu();

    void setDisplayData(std::vector<String> &data);

    void setDisplayDataMainMenu();

    void select();

private:
    ArduinoJson::DynamicJsonDocument jsonStorage;
    std::vector<String> displayData;
    std::vector<std::string> idStorage;
    Menu *mainMenu;
    Menu *currentMenu;
    Adafruit_SH1106 display;
    const String token;
    uint16_t position;
    uint32_t pageCounter;
    uint32_t maxPage;
    uint8_t extraInfoCounter = 0;
};


#endif //ESPACETRADERS_MENUHANDLER_H
