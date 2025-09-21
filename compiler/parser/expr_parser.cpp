#include "expr_nodes.h"
#include "parser.h"
#include "syntax_error.h"
#include "token_error.h"
#include "token_type.h"


ExprPtr Parser::parse_expr() {
    ExprPtr lhs = parse_primary();

    return parse_binary_expr(0, std::move(lhs));
}

ExprPtr Parser::parse_primary() {
    using enum TokenType;
    switch (cur_tok.type) {
        case LParen: return parse_paren_expr();
        case LCurly: return parse_block_expr();
        case Id: return parse_id_expr({});
        case If: return parse_if_else_expr();
        case While: return parse_while_expr();
        case IntConst:
        case FloatConst:
        case CharConst:
        case True:
        case False:
        case StringConst: return parse_constant();
        case Plus:
        case Minus:
        case Not:
            return parse_unary_expr();
        default: throw SyntaxError("Unexpected Token while parsing expression", cur_tok.loc);
    }
}

int get_precedence(const Token& t) {
    using enum TokenType;
    switch (t.type) {
        case Or: return 10;
        case And: return 20;
        case Greater:
        case Less:
        case GreaterEqual:
        case LessEqual:
        case EqualsEquals:
        case NotEquals:
            return 30;
        case Caret:
        case Plus:
        case Minus:
            return 40;
        case Star:
        case Slash:
            return 50;
        default: return -1;
    }
}

bool is_binary_op(const Token& t) {
    using enum TokenType;
    switch (t.type) {
        case Plus:
        case Minus:
        case Star:
        case Slash:
        case Percent:
        case And:
        case Or:
        case Greater:
        case Less:
        case GreaterEqual:
        case LessEqual:
        case EqualsEquals:
        case NotEquals:
        case Caret:
            return true;
            default: return false;
    }
}

ExprPtr Parser::parse_binary_expr(const int precedence, ExprPtr lhs) {
    while (true) {
        Token op = cur_tok;
        const int tok_prec = get_precedence(op);

        if (tok_prec < precedence) {
            return lhs;
        }

        if (!is_binary_op(op)) {
            log_error("Invalid binary operator");
        }
        advance();

        auto rhs = parse_primary();


        if (tok_prec < get_precedence(cur_tok)) {
            rhs = parse_binary_expr(tok_prec + 1, std::move(rhs));
        }

        lhs = std::make_unique<BinaryExpr>(op, std::move(lhs), std::move(rhs));
    }
}


ExprPtr Parser::parse_constant() {
    auto tok = cur_tok;
    advance();
    switch (tok.type) {
        case TokenType::IntConst: return std::make_unique<IntConst>(tok);
        case TokenType::FloatConst: return std::make_unique<FloatConst>(tok);
        case TokenType::CharConst: return std::make_unique<CharConst>(tok);
        case TokenType::StringConst: return std::make_unique<StringConst>(tok);
        case TokenType::True:
        case TokenType::False: return std::make_unique<BoolConst>(tok);
        // Default case won't happen, already checked by parse_expr()
        default: throw SyntaxError("Invalid constant", tok.loc);
    }
}

ExprPtr Parser::parse_unary_expr() {
    const auto op = cur_tok;
    advance();
    return std::make_unique<UnaryExpr>(op, parse_expr());
}

ExprPtr Parser::parse_id_expr(std::optional<Token> id) {
    if (!id.has_value()) {
        id.emplace(cur_tok);
        advance();
    }
    const auto id_tok = id.value();
    if (cur_tok.type == TokenType::LParen) {
        return parse_fun_call(id_tok);
    }
    return std::make_unique<IdExpr>(id_tok);
}

ExprPtr Parser::parse_id(const Token& id) {
    ExprPtr lhs;

    // return parse_binary_expr(0, std::move(lhs));
    if (cur_tok.type != TokenType::LParen) {
        return parse_binary_expr(0, std::make_unique<IdExpr>(id));
    }
    return parse_binary_expr(0, parse_fun_call(id));
}

ExprPtr Parser::parse_fun_call(const Token& id) {
    expect(TokenType::LParen);
    std::vector<ExprPtr> args;
    while (cur_tok.type != TokenType::RParen) {
        args.push_back(parse_expr());

        if (cur_tok.type == TokenType::RParen) {
            break;
        }
        expect(TokenType::Comma, "Expected ')' or ','");
    }
    advance();
    return std::make_unique<FunCall>(id, std::move(args));
}

ExprPtr Parser::parse_paren_expr() {
    const auto loc = cur_tok.loc;
    advance();
    auto expr = parse_expr();
    expect(TokenType::RParen);
    return std::make_unique<ParenExpr>(loc, std::move(expr));
}

ExprPtr Parser::parse_block_expr() {
    const auto loc = cur_tok.loc;
    advance();
    std::vector<StmtPtr> body;
    while (true) {
        skip_new_lines();
        if (cur_tok.type == TokenType::RCurly) {
            break;
        }
        body.push_back(parse_stmt());

    }
    advance();
    return std::make_unique<BlockExpr>(loc, std::move(body));
}

ExprPtr Parser::parse_if_else_expr() {
    const auto loc = cur_tok.loc;
    advance();
    auto condition = parse_expr();
    expect(TokenType::Then);
    skip_new_lines();
    auto if_branch = parse_expr();
    skip_new_lines();
    std::optional<ExprPtr> else_branch = {};
    if (cur_tok.type == TokenType::Else) {
        advance();
        skip_new_lines();
        else_branch.emplace(parse_expr());
    }
    return std::make_unique<IfElseExpr>(loc, std::move(condition),
        std::move(if_branch), std::move(else_branch));
}

ExprPtr Parser::parse_while_expr() {
    const auto loc = cur_tok.loc;
    advance();
    auto condition = parse_expr();
    if (cur_tok.type != TokenType::LCurly) {
        log_error("Expected '{'");
    }
    auto body = parse_expr();
    return std::make_unique<WhileExpr>(loc, std::move(condition), std::move(body));
}

