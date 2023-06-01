//
// Created by gergocs on 2023.05.21..
//

#include "Menu.h"

#include <utility>

Menu::Menu(const std::vector<String> &menuItems, Name name, Menu *parent, bool needStar, String extraInfo) : parent(parent), menuItems(menuItems), name(name), needStar(needStar),
                                                                                                             extraInfo(std::move(extraInfo)) {}

Menu *Menu::getParent() const {
    return parent;
}

const std::vector<String> &Menu::getMenuItems() const {
    return menuItems;
}

const std::vector<Menu *> &Menu::getChilds() const {
    return childs;
}

void Menu::setMenuItems(const std::vector<String> &items) {
    this->menuItems = items;
}

void Menu::addChild(Menu *child) {
    this->childs.push_back(child);
}

Menu::~Menu() {
    for (auto &item: this->childs) {
        delete item;
    }
}

void Menu::clearItems() {
    this->childs.clear();
    this->menuItems.clear();
}

Menu::Name Menu::getName() const {
    return name;
}

bool Menu::isNeedStar() const {
    return needStar;
}

const String &Menu::getExtraInfo() const {
    return extraInfo;
}

void Menu::setExtraInfo(const String &extraInfo) {
    Menu::extraInfo = extraInfo;
}
