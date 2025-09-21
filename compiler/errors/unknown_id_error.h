#pragma once
#include <exception>
#include <string>
#include "error_msg.h"


class UnknownIdError final : public std::exception {
    std::string full_msg;

public:
    UnknownIdError(Location location, const std::string& id)
        : full_msg(token_message(location, "Unknown Identifier: " + id)) {}

    const char* what() const noexcept override {
        return full_msg.c_str();
    }
};