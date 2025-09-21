#pragma once
#include <string>
#include <unordered_map>

enum class TokenType;

std::string str_of_type(TokenType);

bool is_keyword(const std::string&);


enum class TokenType {
    Id,
    IntConst,
    FloatConst,
    CharConst,
    StringConst,
    BoolConst,
    // Keywords
    Internal,
    External,
    Var,
    Let,
    Fun,
    Of,
    True,
    False,
    And,
    Or,
    Not,
    Int,
    Float,
    Char,
    String,
    Unit,
    Bool,
    If,
    Then,
    Else,
    While,



    // Symbols
    Semicolon,
    Comma,
    Colon,
    Dot,
    LParen,
    RParen,
    LCurly,
    RCurly,
    LBracket,
    RBracket,
    Plus,
    Minus,
    Star,
    Slash,
    Percent,
    Equal,
    Caret,
    Less,
    Greater,
    LessEqual,
    GreaterEqual,
    EqualsEquals,
    NotEquals,


    // Utils
    Newline,
    Eof,
    Error,
};

inline std::unordered_map<std::string, TokenType> keywords = {
    {"var", TokenType::Var},
    {"let", TokenType::Let},
    {"fun", TokenType::Fun},
    {"internal", TokenType::Internal},
    {"external", TokenType::External},
    {"of", TokenType::Of},
    {"true", TokenType::True},
    {"false", TokenType::False},
    {"and", TokenType::And},
    {"or", TokenType::Or},
    {"not", TokenType::Not},
    {"int",   TokenType::Int},
    {"float", TokenType::Float},
    {"char",  TokenType::Char},
    {"string",TokenType::String},
    {"bool", TokenType::Bool},
    {"unit", TokenType::Unit},
    {"if", TokenType::If},
    {"then", TokenType::Then},
    {"else", TokenType::Else},
    {"while", TokenType::While}
};;