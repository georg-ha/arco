#pragma once
#include <exception>
#include <string>
#include "error_msg.h"


class SyntaxError: public std::exception {
    std::string full_msg;

public:
    SyntaxError(const std::string& message, Location location)
        : full_msg(token_message(location, "Syntax Error: " + message)) {}

    const char* what() const noexcept override {
        return full_msg.c_str();
    }
};
