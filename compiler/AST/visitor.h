#pragma once

#include "expr.h"
#include "stmt.h"
#include "type_anno.h"


struct Visitor {
    virtual void visit(TypeAnno& typeAnno) = 0;

    virtual void visit(ParenExpr& expr) = 0;

    virtual void visit(BlockExpr& expr) = 0;

    virtual void visit(UnaryExpr& expr) = 0;

    virtual void visit(BinaryExpr& expr) = 0;

    virtual void visit(IdExpr& expr) = 0;

    virtual void visit(IntConst& expr) = 0;

    virtual void visit(FloatConst& expr) = 0;

    virtual void visit(CharConst& expr) = 0;

    virtual void visit(StringConst& expr) = 0;

    virtual void visit(BoolConst& expr) = 0;

    virtual void visit(FunCall& expr) = 0;

    virtual void visit(IfElseExpr& expr) = 0;

    virtual void visit(WhileExpr& expr) = 0;
    
    // Statements

    virtual void visit(ExprStmt& stmt) = 0;

    virtual void visit(VarInit& stmt) = 0;

    virtual void visit(Assignment& stmt) = 0;

    virtual void visit(DefArg& arg) = 0;

    virtual void visit(FunSignature& signature) = 0;

    virtual void visit(FunDef& def) = 0;
    
    virtual void visit(ExternalStmt& stmt) = 0;

    
    virtual ~Visitor() = default;
};
