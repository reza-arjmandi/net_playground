#pragma once

enum class Methods {
    NONE, GET, POST, DELETE2, INSERT, UPDATE
};

enum class Subjects {
    NONE, AUTH_CHALLENGE, BUSINESS_RULE
};

struct Request {

    Request(std::string& method, std::string& subject, std::string data = "") {
        if(_str2method.count(method) == 1) {
            Method = _str2method.at(method);
        }
        if(_str2subjects.count(subject) == 1) {
            Subject = _str2subjects.at(subject);
        }
        Data = data;
    }

    Request() = default;

    void set_method(std::string& method) {
        Method = _str2method[method];
    }

    void set_subject(std::string& subject) {
        Subject = _str2subjects[subject];
    }

    Methods Method;
    Subjects Subject;
    std::string Data = "";

private:

    std::map<std::string, Methods> _str2method{
        {"GET", Methods::GET},
        {"POST", Methods::POST},
        {"DELETE", Methods::DELETE2},
        {"INSERT", Methods::INSERT},
        {"UPDATE", Methods::UPDATE}};
    std::map<std::string, Subjects> _str2subjects{
        {"AUTH_CHALLENGE", Subjects::AUTH_CHALLENGE},
        {"BUSINESS_RULE", Subjects::BUSINESS_RULE}};

};

struct Response {

    Response(std::string subject, std::string data = "") {
        if(_str2subjects.count(subject) == 1) {
            Subject = _str2subjects.at(subject);
        }
        Data = data;
    }

    std::string to_string() {
        return _subjects2str.at(Subject) + " " + Data + "\n";
    }

    Subjects Subject;
    std::string Data = "";

private:

    std::map<std::string, Subjects> _str2subjects{
        {"AUTH_CHALLENGE", Subjects::AUTH_CHALLENGE},
        {"BUSINESS_RULE", Subjects::BUSINESS_RULE}};

    std::map<Subjects, std::string> _subjects2str{
        {Subjects::AUTH_CHALLENGE, "AUTH_CHALLENGE"},
        {Subjects::BUSINESS_RULE, "BUSINESS_RULE"}};

};