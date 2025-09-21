#include <string>
#include "token_type.h"

using enum TokenType;

std::string str_of_type(TokenType type) {
    switch (type) {
        case Id:           return "Identifier";
        case IntConst:     return "Int Constant";
        case FloatConst:   return "Float Constant";
        case CharConst:    return "Char Constant";
        case StringConst:  return "String Constant";
        case BoolConst:    return "Bool Constant";
        case Var:          return "var";
        case Let:          return "let";
        case Fun:          return "fun";
        case Internal:     return "internal";
        case External:     return "external";
        case Of:           return "of";
        case True:         return "true";
        case False:        return "false";
        case And:          return "and";
        case Or:           return "or";
        case Not:          return "not";
        case Int:          return "int";
        case Float:        return "float";
        case Char:         return "char";
        case String:       return "string";
        case Unit:         return "unit";
        case If:           return "if";
        case Then:         return "then";
        case Else:         return "else";
        case While:        return "while";

        case Semicolon:    return ";";
        case Comma:        return ",";
        case Colon:        return ":";
        case Dot:          return ".";
        case LParen:       return "(";
        case RParen:       return ")";
        case LCurly:       return "{";
        case RCurly:       return "}";
        case LBracket:     return "[";
        case RBracket:     return "]";
        case Plus:         return "+";
        case Minus:        return "-";
        case Star:         return "*";
        case Slash:        return "/";
        case Percent:      return "%";
        case Equal:       return "=";
        case Caret:        return "^";
        case Less:         return "<";
        case Greater:      return ">";
        case LessEqual:    return "<=";
        case GreaterEqual: return ">=";
        case EqualsEquals: return "==";
        case NotEquals:    return  "!=";

        case Newline:      return "Newline";
        case Eof:          return "Eof";
        case Error:        return "Error";

        default:           return "Unknown";
    }
}

bool is_keyword(const std::string & lexeme) {
    return keywords.contains(lexeme);
}
