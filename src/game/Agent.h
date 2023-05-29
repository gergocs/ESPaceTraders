//
// Created by gergocs on 2023.05.14..
//

#ifndef ESPACETRADERS_AGENT_H
#define ESPACETRADERS_AGENT_H


#include <Arduino.h>

#define EEPROM_SIZE 514u

class Agent {
public:
    explicit Agent(const String& token);

    const String &getToken() const;

private:
    String token;
};


#endif //ESPACETRADERS_AGENT_H
