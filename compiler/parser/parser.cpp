#include "parser.h"

#include "stmt.h"
#include "stmt_nodes.h"
#include "syntax_error.h"
#include "token_type.h"

std::vector<StmtPtr> parse_file(SourceFile &src) {
    Parser p(src);
    std::vector<StmtPtr> ast;
    while (true) {
        if (auto node = p.parse_stmt()) {
            ast.push_back(std::move(node));
        } else {
            break;
        }
    }
    return ast;
}

Parser::Parser(SourceFile &src)
    : lexer(src), cur_tok(lexer.advance()) {
}

void Parser::advance() {
    cur_tok = lexer.advance();
}

void Parser::expect(TokenType t) {
    if (cur_tok.type != t) {
        log_error("Expected " + str_of_type(t));
    }
    advance();
}

void Parser::expect(TokenType t, const std::string &msg) {
    if (cur_tok.type != t) {
        log_error(msg);
    }
    advance();
}

void Parser::expect_stmt_end() {
    if (cur_tok.type != TokenType::Newline && cur_tok.type != TokenType::Semicolon && cur_tok.type != TokenType::Eof) {
        log_error("Expected ';' or a newline");
    }
    advance();
}

void Parser::log_error(const std::string &msg) const {
    throw SyntaxError(msg, cur_tok.loc);
}

void Parser::skip_new_lines() {
    while (cur_tok.type == TokenType::Newline) {
        advance();
    }
}

TypeAnno Parser::parse_type_anno() {
    const auto loc = cur_tok.loc;
    expect(TokenType::Of);
    auto kind = TypeAnno::Kind::Unit;
    switch (cur_tok.type) {
        case TokenType::Int: kind = TypeAnno::Kind::Int; break;
        case TokenType::Float: kind = TypeAnno::Kind::Float; break;
        case TokenType::Unit: kind = TypeAnno::Kind::Unit; break;
        case TokenType::Char: kind = TypeAnno::Kind::Char; break;
        case TokenType::String: kind = TypeAnno::Kind::String; break;
        case TokenType::Bool: kind = TypeAnno::Kind::Bool; break;
        default: log_error("Invalid type annotation");
    }
    advance();
    return {loc, kind};
}


