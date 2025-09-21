#pragma once
#include <memory>

#include "location.h"


struct CodegenVisitor;
struct Type;
struct Visitor;
struct Scope;

struct Stmt {
    virtual ~Stmt() = default;

    Location loc;
    Scope* parent_scope = nullptr;
    Type* type = nullptr;

    explicit Stmt(const Location &loc): loc(loc) {}

    virtual void accept(Visitor&) = 0;
    virtual void codegen_accept(CodegenVisitor& visitor) = 0;
};

using StmtPtr = std::unique_ptr<Stmt>;

struct VarInit;
struct Assignment;
struct ExprStmt;
struct DefArg;
struct FunSignature;
struct FunDef;
struct ExternalStmt;