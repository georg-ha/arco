#pragma once


struct Location;
struct SourceFile;
struct Token;

class Lexer {
public:
    explicit Lexer(SourceFile& src);

    Token advance();

private:
    int line = 1;
    int column = 1;
    int paren_count = 0;
    SourceFile& src;
    std::string* cur_line;

    char peek() const;
    char get_char();

    void log_syntax_error(const std::string& msg) const;
    Location get_loc_col(int start_col) const;

    Token lex_number();
    Token lex_identifier();
    Token lex_char();
    Token lex_string();
    Token lex_symbol();
};
