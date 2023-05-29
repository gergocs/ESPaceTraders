#include <Arduino.h>
#include <vector>
#include "RESTClient/RESTClient.h"
#include "game/Agent.h"
#include "menuHandler/MenuHandler.h"

#define SDA 21
#define SCL 22

#define FORWARD_BTN 17
#define BACKWARD_BTN 19
#define SELECT_BTN 5
#define BACK_BTN 18

Adafruit_SH1106 display(SDA, SCL);
MenuHandler *menuHandler;

Agent *agent;

void setup() {
    pinMode(FORWARD_BTN, INPUT_PULLUP);
    pinMode(BACKWARD_BTN, INPUT_PULLUP);
    pinMode(SELECT_BTN, INPUT_PULLUP);
    pinMode(BACK_BTN, INPUT_PULLUP);

    Serial.begin(9600);

    ArduinoJson::DynamicJsonDocument jsonData(1024);
    RESTClient::init("big chunky boy", "");

    auto data = RESTClient::Get("https://api.spacetraders.io/v2", "");

    menuHandler = new MenuHandler(display);

    jsonData["faction"] = "COSMIC";
    jsonData["symbol"] = "alma6";
    jsonData["email"] = "alma6@alma6.com";

    jsonData = RESTClient::Post("https://api.spacetraders.io/v2/register", "", jsonData)["data"];

    Serial.println(ArduinoJson::serializeJson(jsonData, Serial));

    if (jsonData["token"].isNull()) {
        agent = new Agent("");
    } else {
        agent = new Agent(jsonData["token"].as<String>());
    }

    menuHandler->setDisplayDataMainMenu();
    menuHandler->renderMenu();
}

void loop() {
    if (digitalRead(FORWARD_BTN) == LOW) {
        menuHandler->forWard();
        menuHandler->renderMenu();
        delay(100);
    } else if (digitalRead(BACKWARD_BTN) == LOW) {
        menuHandler->backWard();
        menuHandler->renderMenu();
        delay(100);
    } else if (digitalRead(SELECT_BTN) == LOW) {
        menuHandler->select(agent->getToken());
        menuHandler->renderMenu();
        delay(100);
    } else if (digitalRead(BACK_BTN) == LOW) {
        menuHandler->prevMenu();
        menuHandler->renderMenu();
        delay(100);
    }
}