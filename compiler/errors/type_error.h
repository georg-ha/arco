#pragma once
#include <exception>
#include <string>
#include "error_msg.h"


class TypeError final : public std::exception {
    std::string full_msg;

public:
    TypeError(Location& location, const std::string& msg)
        : full_msg(token_message(location, "Type Error: " + msg)) {}

    const char* what() const noexcept override {
        return full_msg.c_str();
    }
};
