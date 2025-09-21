#pragma once
#include <memory>
#include "location.h"

namespace llvm {
class Value;
}

struct CodegenVisitor;
struct Type;
struct Visitor;
struct Scope;


struct Expr {
    virtual ~Expr() = default;

    Location loc;
    Scope* parent_scope = nullptr;
    Type* type = nullptr;

    explicit Expr(const Location &loc): loc(loc) {}

    virtual void accept(Visitor&) = 0;
    virtual llvm::Value* codegen_accept(CodegenVisitor& visitor) = 0;
};

using ExprPtr = std::unique_ptr<Expr>;

struct ParenExpr;
struct UnaryExpr;
struct IdExpr;
struct BinaryExpr;
struct FunCall;
struct BlockExpr;
struct IfElseExpr;
struct WhileExpr;

struct IntConst;
struct FloatConst;
struct CharConst;
struct StringConst;
struct BoolConst;