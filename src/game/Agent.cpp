//
// Created by gergocs on 2023.05.14..
//

#include <EEPROM.h>
#include "Agent.h"

Agent::Agent(const String& token) {
    if (!EEPROM.begin(std::max(EEPROM_SIZE, token.length()))) {
        Serial.println("failed to init EEPROM");
    }

    for (int i = 0; i < std::max(EEPROM_SIZE, token.length()); i++) {
        byte readValue = EEPROM.read(i);

        if (readValue == 0) {
            break;
        }

        char readValueChar = char(readValue);
        this->token += readValueChar;
        Serial.print(readValueChar);
    }

    if (this->token == token || token.isEmpty()) {
        return;
    }

    for (int i = 0; i < std::max(EEPROM_SIZE, token.length()); i++) {
        EEPROM.write(i, token[i]);
    }

    EEPROM.commit();

    this->token = token;
}

const String &Agent::getToken() const {
    return token;
}
