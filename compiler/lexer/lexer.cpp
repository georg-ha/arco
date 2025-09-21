#include <string>
#include "lexer.h"
#include "source_file.h"
#include "token.h"
#include "token_type.h"
#include "syntax_error.h"

Lexer::Lexer(SourceFile &src)
    : src(src), cur_line(&src.get_line(1)){
}

Token Lexer::advance() {
    while (true) {
        const char c = peek();
        if (c == '\0') {
            get_char();
            return {TokenType::Eof, "", get_loc_col(column)};
        }

        if (c == '#') {
            const int co = static_cast<int>(cur_line->size());
            const Location location{src, line,co,co};
            line++;
            column = 0;
            return {TokenType::Newline, "", location};
        }


        if (std::isspace(c)) {
            const int start_col = column;
            get_char();
            if (c == '\n' && paren_count == 0) {
                return {TokenType::Newline, "\n", get_loc_col(start_col)};
            }
            continue;
        }

        if (std::isdigit(c)) {
            return lex_number();
        }

        if (std::isalpha(c)) {
            return lex_identifier();
        }

        if (c == '\'') {
            return lex_char();
        }

        if (c == '\"') {
            return lex_string();
        }

        if (std::ispunct(c)) {
            return lex_symbol();
        }
    }
}

char Lexer::peek() const {
    if (column > cur_line->size()) {
        if (line + 1 > src.length()) {
            return '\0';
        }
        return src.get_line(line+1)[0];
    }
    return (*cur_line)[column-1];
}

Location Lexer::get_loc_col(int start_col) const {
    // Ensures token at the beginning of a line have start_col = 1
    start_col = column < start_col ? 1 : start_col;
    return {src, line, start_col, column};
}

char Lexer::get_char() {
    if (column > cur_line->size()) {
        column = 1;
        line++;
        if (line > src.length()) {
            return '\0';
        }
        cur_line = &src.get_line(line);
    }
    return (*cur_line)[column++ -1];
}

void Lexer::log_syntax_error(const std::string &msg) const {
    throw SyntaxError(msg, {get_loc_col(column)});
}


Token Lexer::lex_number() {
    const int col_start = column;
    std::string lexeme;
    auto t = TokenType::IntConst;
    while (std::isdigit(peek())) {
        lexeme += get_char();
    }
    if (peek() == '.') {
        t = TokenType::FloatConst;
        do {
            lexeme += get_char();
        } while (std::isdigit(peek()));
    }
    return {t, lexeme, get_loc_col(col_start)};
}

Token Lexer::lex_identifier() {
    const int col_start = column;
    std::string lexeme;
    do {
        lexeme += get_char();
    } while (std::isalnum(peek()) || peek() == '_');
    const auto t = is_keyword(lexeme) ? keywords.at(lexeme) : TokenType::Id;
    return {t, t == TokenType::Id ? lexeme : "",get_loc_col(col_start)};
}

static const std::unordered_map<char, std::string> escapeSequences = {
    {'n',  "\n"},
    {'t',  "\t"},
    {'r',  "\r"},
    {'\'', "\'"},
    {'\"', "\""},
    {'\\', "\\"},
    {'0',  "\0"},
    {'b',  "\b"},
    {'f',  "\f"},
    {'v',  "\v"}
};

Token Lexer::lex_char() {
    const int start_col = column;
    get_char(); // '
    const char c = peek();

    if (c == '\n') {
        log_syntax_error("Unterminated char literal");
    }
    std::string lexeme;
    if (c == '\\') {
        get_char();
        if (escapeSequences.contains(peek())) {
            lexeme += escapeSequences.at(get_char());
        } else {
            log_syntax_error("Unknown escape character");
        }
    } else if (c == '\'') {
        log_syntax_error("empty char literal");
    } else {
        lexeme += get_char();
    }
    get_char(); // '
    return {TokenType::CharConst, lexeme, get_loc_col(start_col)};
}

Token Lexer::lex_string() {
    const int start_col = column;
    get_char(); // "
    std::string lexeme;
    while (peek() != '\"') {
        const char c = peek();

        if (c == '\n') {
            log_syntax_error("Unterminated string literal");
        }
        if (c == '\\') {
            get_char();
            if (escapeSequences.contains(peek())) {
                lexeme += escapeSequences.at(get_char());
                continue;
            }
            log_syntax_error("Unknown escape character");
        } else {
            get_char();
        }

        lexeme += c;
    }
    get_char(); // "
    return {TokenType::StringConst, lexeme, get_loc_col(start_col)};
}


Token Lexer::lex_symbol() {
    const int start_col = column;
    char c = get_char();
    using enum TokenType;
    auto type = Error;
    switch (c) {
        case '(': paren_count++;type = LParen; break;
        case ')': paren_count--;type = RParen; break;
        case '{': type = LCurly; break;
        case '}': type = RCurly; break;
        case '[': paren_count++;type = LBracket; break;
        case ']': paren_count--;type = RBracket; break;
        case ',': type = Comma; break;
        case ';': type = Semicolon; break;
        case '+': type = Plus; break;
        case '-': type = Minus; break;
        case '%': type = Percent; break;
        case '^': type = Caret; break;
        case '.': type = Dot; break;
        case '*': type = Star; break;
        case '/': type = Slash; break;
        case '=': if (peek() == '=') {
            get_char();
            type = EqualsEquals; break;
        } else {
            type = Equal; break;
        }
        case '!': if (peek() != '=') {
            log_syntax_error("Expected '='");
        } else {
            get_char();
            type = NotEquals; break;
        }
        case '>':
            if (peek() != '=') {
                type = Greater; break;
            } else {
                get_char();
                type = GreaterEqual; break;
            }
        case '<':
            if (peek() != '=') {
                type = Less; break;
            } else {
                get_char();
                type = LessEqual; break;
            }
        default: log_syntax_error("Unknown symbol");

    }
    return {type, "", get_loc_col(start_col)};
}
