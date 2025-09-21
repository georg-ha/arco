#pragma once
#include <vector>
#include "expr.h"
#include "stmt.h"
#include "token.h"
#include "token_type.h"
#include "scope.h"


struct ParenExpr final : Expr {
    ExprPtr expr;
    ParenExpr(const Location& loc, ExprPtr expr)
        : Expr(loc), expr(std::move(expr)) {}

    void accept(Visitor &visitor) override;
    llvm::Value* codegen_accept(CodegenVisitor& visitor) override;
};

enum class UnaryOp {Minus, Plus, Not};

std::string str_of_unary_op(UnaryOp op);

static UnaryOp unary_op_of_type(const Token& t) {
    switch (t.type) {
        case TokenType::Plus: return UnaryOp::Plus;
        case TokenType::Minus: return UnaryOp::Minus;
        case TokenType::Not  : return UnaryOp::Not;
        default: throw std::runtime_error("Invalid unary operand");
    }
}


struct UnaryExpr final : Expr {
    UnaryOp op;
    ExprPtr operand;

    explicit UnaryExpr(const Token& op, ExprPtr operand)
        : Expr(op.loc), op(unary_op_of_type(op)), operand(std::move(operand)) {}

    void accept(Visitor &visitor) override;
    llvm::Value* codegen_accept(CodegenVisitor& visitor) override;
};

enum class BinaryOp {
    Add, Sub, Mul, Div, Mod,
    Concat,
    Equals, NotEquals,
    And, Or,
    Greater, Less, GreaterEquals, LessEquals
};

std::string str_of_binary_op(BinaryOp op);

static BinaryOp binary_op_of_type(const Token& t) {
    switch (t.type) {
        case TokenType::Plus: return BinaryOp::Add;
        case TokenType::Minus: return BinaryOp::Sub;
        case TokenType::Star: return BinaryOp::Mul;
        case TokenType::Slash: return BinaryOp::Div;
        case TokenType::Percent: return BinaryOp::Mod;
        case TokenType::EqualsEquals: return BinaryOp::Equals;
        case TokenType::NotEquals: return BinaryOp::NotEquals;
        case TokenType::Less: return BinaryOp::Less;
        case TokenType::Greater: return BinaryOp::Greater;
        case TokenType::LessEqual: return BinaryOp::LessEquals;
        case TokenType::GreaterEqual: return BinaryOp::GreaterEquals;
        case TokenType::Caret: return BinaryOp::Concat;
        case TokenType::And: return BinaryOp::And;
        case TokenType::Or: return BinaryOp::Or;
        default: throw std::runtime_error("Invalid binary operand");
    }
}

struct IdExpr final : Expr {
    std::string id;
    explicit IdExpr(const Token& id)
        : Expr(id.loc), id(id.lexeme) {}

    void accept(Visitor &visitor) override;
    llvm::Value* codegen_accept(CodegenVisitor& visitor) override;
};


struct BinaryExpr final : Expr {
    BinaryOp op;
    ExprPtr lhs;
    ExprPtr rhs;

    BinaryExpr(const Token& op, ExprPtr lhs, ExprPtr rhs)
        : Expr(op.loc), op(binary_op_of_type(op)), lhs(std::move(lhs)), rhs(std::move(rhs)) {}

    void accept(Visitor &visitor) override;
    llvm::Value* codegen_accept(CodegenVisitor& visitor) override;
};


struct FunCall final : Expr {
    std::string callee;
    std::vector<ExprPtr> args;

    FunCall(const Token& callee, std::vector<ExprPtr> args)
        : Expr(callee.loc), callee(callee.lexeme), args(std::move(args)) {}

    void accept(Visitor &visitor) override;
    llvm::Value* codegen_accept(CodegenVisitor& visitor) override;
};

struct BlockExpr final : Expr {
    std::vector<StmtPtr> body;
    Scope scope;

    BlockExpr(const Location& loc, std::vector<StmtPtr> body)
        : Expr(loc), body(std::move(body)), scope(Scope::Kind::Block) {
    }

    void accept(Visitor &visitor) override;
    llvm::Value* codegen_accept(CodegenVisitor& visitor) override;
};


struct IfElseExpr final : Expr {
    ExprPtr condition;
    ExprPtr if_branch;
    std::optional<ExprPtr> else_branch;

    IfElseExpr(const Location& loc, ExprPtr condition, ExprPtr if_branch, std::optional<ExprPtr> else_branch)
        : Expr(loc), condition(std::move(condition)), if_branch(std::move(if_branch)),
        else_branch(std::move(else_branch)) {}

    void accept(Visitor &visitor) override;
    llvm::Value* codegen_accept(CodegenVisitor& visitor) override;
};


struct WhileExpr final : Expr {
    ExprPtr condition;
    ExprPtr body;

    WhileExpr(const Location& loc, ExprPtr condition, ExprPtr body)
        : Expr(loc), condition(std::move(condition)), body(std::move(body)) {}


    void accept(Visitor &visitor) override;
    llvm::Value* codegen_accept(CodegenVisitor& visitor) override;
};


struct IntConst final : Expr {
    int val;

    explicit IntConst(const Token& tok)
        : Expr(tok.loc), val(std::stoi(tok.lexeme)) {}

    void accept(Visitor &visitor) override;
    llvm::Value* codegen_accept(CodegenVisitor& visitor) override;
};

struct FloatConst final: Expr {
    double val;

    explicit FloatConst(const Token& tok)
        : Expr(tok.loc), val(std::stod(tok.lexeme)) {}

    void accept(Visitor &visitor) override;
    llvm::Value* codegen_accept(CodegenVisitor& visitor) override;
};

struct CharConst final: Expr {
    char val;

    explicit CharConst(const Token& tok)
        : Expr(tok.loc), val(tok.lexeme[0]) {}

    void accept(Visitor &visitor) override;
    llvm::Value* codegen_accept(CodegenVisitor& visitor) override;
};

struct StringConst final: Expr {
    std::string val;

    explicit StringConst(const Token& tok)
        : Expr(tok.loc), val(tok.lexeme) {}

    void accept(Visitor &visitor) override;
    llvm::Value* codegen_accept(CodegenVisitor& visitor) override;
};

struct BoolConst final : Expr {
    bool val;

    explicit BoolConst(const Token& tok)
        : Expr(tok.loc), val(tok.type == TokenType::True) {}

    void accept(Visitor &visitor) override;
    llvm::Value* codegen_accept(CodegenVisitor& visitor) override;
};