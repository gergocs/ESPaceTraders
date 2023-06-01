//
// Created by gergocs on 2023.06.01..
//

#include "JSONEnum.h"

std::string getEnumAsString(JSONEnum val) {
    switch (val) {
        case TOKEN:
            return "token";
        case ACCOUNT_ID:
            return "accountId";
        case FACTION:
            return "faction";
        case SYMBOL:
            return "symbol";
        case EMAIL:
            return "email";
        case DATA:
            return "data";
        case HEADQUARTERS:
            return "headquarters";
        case CREDITS:
            return "credits";
        case META:
            return "meta";
        case TOTAL:
            return "total";
        case PAGE:
            return "page";
        case ID:
            return "id";
        case NAME:
            return "name";
        case IS_RECRUITING:
            return "isRecruiting";
        case TERMS:
            return "terms";
        case DEADLINE:
            return "deadline";
        case PAYMENT:
            return "payment";
        case ON_ACCEPTED:
            return "onAccepted";
        case ON_FULFILLED:
            return "onFulfilled";
        case DELIVER:
            return "deliver";
        case DESTINATION_SYMBOL:
            return "destinationSymbol";
        case UNITS_FULFILLED:
            return "unitsFulfilled";
        case UNITS_REQUIRED:
            return "unitsRequired";
        case FACTION_SYMBOL:
            return "factionSymbol";
        case TYPE:
            return "type";
        case DEADLINE_TO_ACCEPT:
            return "deadlineToAccept";
        case CONTRACT:
            return "contract";
        case CONTRACT_ID:
            return "contractId";
        case ACCEPTED:
            return "accepted";
        case FULFILLED:
            return "fulfilled";
        case TRADE_SYMBOL:
            return "tradeSymbol";
        case ERROR:
            return "error";
        case MESSAGE:
            return "message";
        case DESCRIPTION:
            return "description";
        default:
            return "";
    }
}
