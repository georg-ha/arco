#pragma once
#include <iostream>

#include "type_anno.h"
#include "visitor.h"

struct PrintVisitor final : Visitor {
    int indent{0};
    int tabSize{2};

    void printIndent() const{
        for (int i{0}; i < indent * tabSize; i++) {
            std::cout << " ";
        }
    }

    void visit(TypeAnno &typeAnno) override;

    void visit(ParenExpr &expr) override;

    void visit(BlockExpr &expr) override;

    void visit(UnaryExpr &expr) override;

    void visit(BinaryExpr &expr) override;

    void visit(IdExpr &expr) override;

    void visit(IntConst &expr) override;

    void visit(FloatConst &expr) override;

    void visit(CharConst &expr) override;

    void visit(StringConst &expr) override;

    void visit(BoolConst &expr) override;

    void visit(FunCall &expr) override;

    void visit(IfElseExpr& expr) override;

    void visit(WhileExpr &expr) override;


    // Statements

    void visit(ExprStmt &stmt) override;

    void visit(VarInit &stmt) override;

    void visit(Assignment &stmt) override;

    void visit(DefArg &arg) override;

    void visit(FunSignature &header) override;

    void visit(FunDef &def) override;

    void visit(ExternalStmt& stmt) override;
};
