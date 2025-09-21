#pragma once
#include <optional>
#include <utility>
#include <vector>
#include "expr.h"
#include "stmt.h"
#include "token.h"
#include "type_anno.h"
#include "scope.h"


struct VarInit final : Stmt {
    bool is_internal;
    bool is_const;
    std::string id;
    std::optional<TypeAnno> anno;
    ExprPtr val;

    VarInit(const Location& loc,
        bool is_internal, bool is_const,
        std::string id, const std::optional<TypeAnno>& type_anno, ExprPtr val)
        : Stmt(loc),
        is_internal(is_internal), is_const(is_const),
        id(std::move(id)), anno(type_anno), val(std::move(val)) {}

    void accept(Visitor &visitor) override;
    void codegen_accept(CodegenVisitor& visitor) override;

};

struct Assignment final : Stmt {
    std::string assignee;
    ExprPtr val;

    Assignment(Token& assignee, ExprPtr val)
        : Stmt(assignee.loc), assignee(std::move(assignee.lexeme)), val(std::move(val)) {}

    void accept(Visitor &visitor) override;
    void codegen_accept(CodegenVisitor& visitor) override;
};

struct ExprStmt final : Stmt {
    ExprPtr expr;

    explicit ExprStmt(const Location& loc, ExprPtr expr)
        : Stmt(loc), expr(std::move(expr)) {}

    void accept(Visitor &visitor) override;
    void codegen_accept(CodegenVisitor& visitor) override;
};


struct DefArg {
    Location loc;
    std::string id;
    TypeAnno anno;
    Scope* parent_scope = nullptr;

    DefArg(const Token& id, const TypeAnno& anno)
        : loc(id.loc), id(id.lexeme), anno(anno) {}

    void accept(Visitor &visitor);
};

struct FunSignature {
    Location loc;
    bool var_arg = false;
    std::string id;
    std::vector<DefArg> args;
    TypeAnno anno;
    Scope* parent_scope = nullptr;
    Type* type;

    void accept(Visitor &visitor);
};

struct FunDef final : Stmt {
    bool is_internal;
    FunSignature signature;
    ExprPtr body;
    Scope scope;

    FunDef(const Location& loc, bool is_internal, FunSignature sig, ExprPtr body)
        : Stmt(loc), is_internal(is_internal),
          signature(std::move(sig)), body(std::move(body)),
            scope(Scope::Kind::Function){}

    void accept(Visitor &visitor) override;
    void codegen_accept(CodegenVisitor& visitor) override;
};

struct ExternalStmt final : Stmt {
    FunSignature signature;

    ExternalStmt(const Location& loc, FunSignature sig)
        : Stmt(loc), signature(std::move(sig)) {}

    void accept(Visitor &visitor) override;
    void codegen_accept(CodegenVisitor& visitor) override;
};