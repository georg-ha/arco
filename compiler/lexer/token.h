#pragma once
#include <string>

#include "location.h"


enum class TokenType;

struct Token {
    TokenType type;
    std::string lexeme;
    Location loc;

    std::string to_string() const;
};