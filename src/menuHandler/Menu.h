//
// Created by gergocs on 2023.05.21..
//

#ifndef ESPACETRADERS_MENU_H
#define ESPACETRADERS_MENU_H


#include <Arduino.h>
#include <vector>

class Menu {

public:
    enum Name {
        MainMenu,
        AgentMenu,
        FactionMenu,
        FleetMenu
    };
    explicit Menu(const std::vector<String> &menuItems, Name name = MainMenu, Menu *parent = nullptr);

    Menu *getParent() const;

    const std::vector<String> &getMenuItems() const;

    const std::vector<Menu *> &getChilds() const;

    void setMenuItems(const std::vector<String> &items);

    void addChild(Menu *child);

    void clearItems();

    Name getName() const;

    virtual ~Menu();

private:
    Name name = MainMenu;
    Menu *parent = nullptr;
    std::vector<String> menuItems;
    std::vector<Menu *> childs;
};


#endif //ESPACETRADERS_MENU_H
