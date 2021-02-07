#pragma once

#include "net/Session.h"
#include "net/Permissions.h"

class SessionFactory {

public:

    template<class IOObjectType>
    decltype(auto) operator()(IOObjectType&& io_object) {
        Permissions permission;
        permission.give_permission(Subjects::AUTH_CHALLENGE);
        permission.give_permission(Methods::POST);
        permission.give_permission(Methods::GET);
        return std::make_shared<Session<IOObjectType>>(
                std::forward<IOObjectType>(io_object), permission);
    }

};