#pragma once
#include <exception>
#include <string>

#include "error_msg.h"

class DoubleDefinitionError final : public std::exception {
    std::string full_msg;

public:
    DoubleDefinitionError(const std::string& id, const Location& loc)
        : full_msg(token_message(loc, id + " has already been defined")) {}

    const char* what() const noexcept override {
        return full_msg.c_str();
    }
};
