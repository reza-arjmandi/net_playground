#pragma once

#include "net/Request.h"

class Permissions {

public:

    void give_permission(Methods method) {
        _allow_methods.insert(method);
    }

    void give_permission(Subjects subject) {
        _allow_subjects.insert(subject);
    }

    bool is_allowed(const std::string& method, const std::string& subject){
        return _allow_methods.count(_str2method[method]) == 1
            && _allow_subjects.count(_str2subjects[subject]) == 1;
    }

    bool is_allowed(const Subjects& subject) {
        return _allow_subjects.count(subject) == 1;
    }

    bool is_allowed(const Methods& method) {
        return _allow_methods.count(method) == 1;
    }

private:

    std::map<std::string, Methods> _str2method {
        {"GET", Methods::GET},
        {"POST", Methods::POST},
        {"DELETE", Methods::DELETE2},
        {"INSERT", Methods::INSERT},
        {"UPDATE", Methods::UPDATE}};
    std::map<std::string, Subjects> _str2subjects{
        {"AUTH_CHALLENGE", Subjects::AUTH_CHALLENGE},
        {"BUSINESS_RULE", Subjects::BUSINESS_RULE}};
    std::set<Methods> _allow_methods;
    std::set<Subjects> _allow_subjects;


};