#include <optional>
#include "stmt_nodes.h"
#include "parser.h"
#include "syntax_error.h"
#include "token_type.h"

StmtPtr Parser::parse_stmt() {
    using enum TokenType;
    switch (cur_tok.type) {
        case Eof: return nullptr;
        case Newline: advance();
            return parse_stmt();
        case Internal: return parse_internal_stmt();
        case Let:
        case Var: return parse_var_init({});
        case Fun: return parse_fun_def({});
        case Id: return parse_id_stmt();
        case External: return parse_external_stmt();
        default: return parse_expr_stmt();
    }
}



StmtPtr Parser::parse_id_stmt() {
    auto id = cur_tok;
    advance();
    if (cur_tok.type == TokenType::Equal) {
        return parse_assignment(id);
    }
    auto expr = parse_id(id);
    expect_stmt_end();
    return std::make_unique<ExprStmt>(expr->loc, std::move(expr));

}

StmtPtr Parser::parse_assignment(Token& id) {
    advance();
    auto val = parse_expr();
    expect_stmt_end();
    return std::make_unique<Assignment>(id, std::move(val));
}

StmtPtr Parser::parse_var_init(std::optional<Location> loc) {
    bool is_internal = true;
    if (!loc.has_value()) {
        loc.emplace(cur_tok.loc);
        is_internal = false;
    }
    const auto is_const = cur_tok.type == TokenType::Let;
    advance();
    const auto id = cur_tok;
    expect(TokenType::Id);
    std::optional<TypeAnno> anno;
    if (cur_tok.type == TokenType::Of) {
        anno.emplace(parse_type_anno());
    }
    expect(TokenType::Equal);
    auto val = parse_expr();
    expect_stmt_end();
    return std::make_unique<VarInit>(loc.value(), is_internal, is_const, id.lexeme, anno, std::move(val));
}


StmtPtr Parser::parse_fun_def(std::optional<Location> loc) {
    bool is_internal = true;
    if (!loc.has_value()) {
        loc.emplace(cur_tok.loc);
        is_internal = false;
    }
    FunSignature signature = parse_fun_sig();
    expect(TokenType::Equal);
    auto body = parse_expr();
    expect_stmt_end();
    return std::make_unique<FunDef>(loc.value(), is_internal, signature, std::move(body));
}

DefArg Parser::parse_def_arg() {
    const auto tok = cur_tok;
    advance();
    return {tok, parse_type_anno()};
}

FunSignature Parser::parse_fun_sig() {
    advance();
    const auto id = cur_tok;
    expect(TokenType::Id);
    expect(TokenType::LParen);
    std::vector<DefArg> args;
    while (cur_tok.type != TokenType::RParen) {
        args.push_back(parse_def_arg());
        if (cur_tok.type == TokenType::RParen) {
            break;
        }
        expect(TokenType::Comma, "Expected ',' or ')'");
    }
    advance();
    return {id.loc, false, id.lexeme, args, parse_type_anno()};
}

StmtPtr Parser::parse_internal_stmt() {
    const auto loc = cur_tok.loc;
    advance();
    switch (cur_tok.type) {
        case TokenType::Fun: return parse_fun_def(loc);
        case TokenType::Var:
        case TokenType::Let: return parse_var_init(loc);
        default: throw SyntaxError(
            "Only function definitions and variable definitions can be marked as internal", cur_tok.loc);
    }
}

StmtPtr Parser::parse_expr_stmt() {
    auto expr = parse_expr();
    expect_stmt_end();
    return std::make_unique<ExprStmt>(expr->loc, std::move(expr));
}

StmtPtr Parser::parse_external_stmt() {
    const auto loc = cur_tok.loc;
    advance();
    if (cur_tok.type != TokenType::Fun) {
        log_error("Only function declarations are allowed after 'external'");
    }
    FunSignature sig = parse_fun_sig();
    expect_stmt_end();
    return std::make_unique<ExternalStmt>(loc, sig);
}
