#include "token.h"

#include "token_type.h"

std::string Token::to_string() const {
    return loc.to_string() + ":: " + str_of_type(type) + " " + lexeme;
}
