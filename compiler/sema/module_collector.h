#pragma once
#include "scope.h"
#include "visitor.h"

// 1. Collects function signatures so that functions can be called before they are declared
// 2. Ensures that only allowed statements are used at the module level


struct ModuleCollector final : Visitor {
    Scope& mod_scope;

    explicit ModuleCollector(Scope& mod_scope)
        : mod_scope(mod_scope) {}

    void visit(FunDef &def) override;
    void visit(ExternalStmt &stmt) override;

    // Empty Implementations
    void visit(TypeAnno &typeAnno) override;

    void visit(ParenExpr &expr) override;

    void visit(BlockExpr &expr) override;

    void visit(UnaryExpr &expr) override;

    void visit(BinaryExpr &expr) override;

    void visit(IdExpr &expr) override;

    void visit(IfElseExpr &expr) override;

    void visit(WhileExpr &expr) override;

    void visit(IntConst &expr) override;

    void visit(FloatConst &expr) override;

    void visit(CharConst &expr) override;

    void visit(StringConst &expr) override;

    void visit(BoolConst &expr) override;

    void visit(FunCall &expr) override;

    void visit(ExprStmt &stmt) override;

    void visit(VarInit &stmt) override;

    void visit(Assignment &stmt) override;

    void visit(DefArg &arg) override;

    void visit(FunSignature &signature) override;
};