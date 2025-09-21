#pragma once
#include <optional>
#include <vector>

#include "lexer.h"
#include "expr.h"
#include "stmt.h"
#include "token.h"

struct DefArg;
struct FunSignature;
struct TypeAnno;


std::vector<StmtPtr> parse_file(SourceFile& src);

struct Parser {
    explicit Parser(SourceFile& src);

    StmtPtr parse_stmt();
    ExprPtr parse_expr();

private:
    Lexer lexer;
    Token cur_tok;

    void advance();
    void expect(TokenType t);
    void expect(TokenType t, const std::string& msg);
    void expect_stmt_end();
    void log_error(const std::string& msg) const;
    void skip_new_lines();

    TypeAnno parse_type_anno();

    // Statements

    // loc has a value when the definition internal
    StmtPtr parse_var_init(std::optional<Location> loc);
    StmtPtr parse_fun_def(std::optional<Location> loc);
    StmtPtr parse_id_stmt();
    StmtPtr parse_assignment(Token&);
    DefArg parse_def_arg();
    FunSignature parse_fun_sig();
    StmtPtr parse_internal_stmt();
    StmtPtr parse_expr_stmt();
    StmtPtr parse_external_stmt();

    // Expressions

    ExprPtr parse_primary();
    ExprPtr parse_constant();
    ExprPtr parse_unary_expr();
    ExprPtr parse_binary_expr(int precedence, ExprPtr lhs);
    ExprPtr parse_id_expr(std::optional<Token> id);
    ExprPtr parse_id(const Token& id);
    ExprPtr parse_fun_call(const Token& id);
    ExprPtr parse_paren_expr();
    ExprPtr parse_block_expr();
    ExprPtr parse_if_else_expr();
    ExprPtr parse_while_expr();

};
