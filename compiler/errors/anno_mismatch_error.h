#pragma once
#include <exception>
#include <string>
#include "error_msg.h"
#include "stmt_nodes.h"



class AnnoMismatchError final : public std::exception {
    std::string full_msg;

public:
    explicit AnnoMismatchError(VarInit& var_init)
        : full_msg(token_message(var_init.loc,
            "The type annotation and the assigned expression don't have the same type")) {}

    const char* what() const noexcept override {
        return full_msg.c_str();
    }
};
