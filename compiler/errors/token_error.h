#pragma once
#include <exception>
#include <string>
#include "error_msg.h"


class TokenError final : public std::exception {
    std::string full_msg;

public:
    TokenError(const std::string& message, Location location)
        : full_msg(token_message(location, message)) {}

    const char* what() const noexcept override {
        return full_msg.c_str();
    }
};
