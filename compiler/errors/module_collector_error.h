#pragma once
#include <string>
#include <exception>

#include "stmt.h"
#include "error_msg.h"

class ModuleCollectorError final : public std::exception {
    std::string full_msg;

public:
    explicit ModuleCollectorError(Stmt& stmt)
        : full_msg(token_message(stmt.loc, "Can't declare this at the module level")) {}

    const char* what() const noexcept override {
        return full_msg.c_str();
    }
};
