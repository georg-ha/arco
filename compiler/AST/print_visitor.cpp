#include "print_visitor.h"

#include <algorithm>

#include "expr_nodes.h"
#include "stmt_nodes.h"



void PrintVisitor::visit(TypeAnno& typeAnno) {
    printIndent();
    std::string str;
    switch (typeAnno.kind) {
        using enum TypeAnno::Kind;
        case Int: str = "int"; break;
        case Float: str = "float"; break;
        case Char: str = "char"; break;
        case String: str = "string"; break;
        case Bool: str = "bool"; break;
        case Unit: str = "unit"; break;
    }
    std::cout << "TypeAnno: " << str << "\n";
}

void PrintVisitor::visit(ParenExpr &expr)  {
    printIndent();
    std::cout << "ParenExpr:\n";
    indent++;
    expr.expr->accept(*this);
    indent--;
}

void PrintVisitor::visit(BlockExpr &expr)  {
    printIndent();
    std::cout << "BlockExpr: \n";
    indent++;
    for (const auto& stmt : expr.body) {
        stmt->accept(*this);
    }
    indent--;
}

void PrintVisitor::visit(UnaryExpr &expr)  {
    printIndent();
    std::cout << "UnaryExpr: " << str_of_unary_op(expr.op) << "\n";
    indent++;
    expr.operand->accept(*this);
    indent--;
}

void PrintVisitor::visit(BinaryExpr &expr)  {
    printIndent();
    std::cout << "BinaryExpr:\n";
    indent++;
    expr.lhs->accept(*this);
    indent--;
    printIndent();
    std::cout << str_of_binary_op(expr.op) << "\n";
    indent++;
    expr.rhs->accept(*this);
    indent--;
}

void PrintVisitor::visit(IdExpr &expr) {
    printIndent();
    std::cout << "IdExpr: " + expr.id + "\n";
}

void PrintVisitor::visit(IntConst &expr) {
    printIndent();
    std::cout << "IntConst: " << expr.val << "\n";
}

void PrintVisitor::visit(FloatConst &expr) {
    printIndent();
    std::cout << "FloatConst: " << expr.val << "\n";
}

void PrintVisitor::visit(CharConst &expr) {
    printIndent();
    std::cout << "CharConst: " << expr.val << "\n";
}

void PrintVisitor::visit(StringConst &expr) {
    printIndent();
    std::cout << "StringConst: " << expr.val << "\n";
}

void PrintVisitor::visit(BoolConst &expr) {
    printIndent();
    std::cout << "BoolConst: " << expr.val << "\n";
}

void PrintVisitor::visit(FunCall &expr)  {
    printIndent();
    std::cout << "FuncCall: " << expr.callee << "\n";
    indent++;
    for (const auto& arg : expr.args) {
        arg->accept(*this);
    }
    indent--;
}

void PrintVisitor::visit(IfElseExpr &expr) {
    printIndent();
    std::cout << "IfElseExpr: If\n";
    indent++;
    expr.condition->accept(*this);
    indent--;
    printIndent();
    std::cout << "IfBranch:\n";
    indent++;
    printIndent();
    expr.if_branch->accept(*this);
    indent--;
    if (expr.else_branch.has_value()) {
        printIndent();
        std::cout << "ElseBranch:\n";
        indent++;
        expr.else_branch.value()->accept(*this);
        indent--;
    }
}

void PrintVisitor::visit(WhileExpr &expr) {
    printIndent();
    std::cout << "WhileExpr:\n";
    indent++;
    expr.condition->accept(*this);
    printIndent();
    expr.body->accept(*this);
    indent--;
}


void PrintVisitor::visit(ExprStmt &stmt)  {
    printIndent();
    std::cout << "(ExprStmt: \n";
    indent++;
    stmt.expr->accept(*this);
    indent--;
    printIndent();
    std::cout << ")\n";
}

// Statements

void PrintVisitor::visit(VarInit &stmt)  {
    printIndent();
    std::cout << (stmt.is_const ? "let" : "var") << ": " + stmt.id << "\n";
    indent++;
    if (stmt.anno.has_value()) {
        stmt.anno->accept(*this);
    }
    indent--;
    printIndent();
    std::cout << "=\n";
    indent++;
    stmt.val->accept(*this);
    indent--;
}

void PrintVisitor::visit(Assignment &stmt)  {
    printIndent();
    std::cout << "Assignment:\n";
    indent++;
    printIndent();
    std::cout << stmt.assignee << "\n";
    indent--;
    printIndent();
    std::cout << "=\n";
    indent++;
    stmt.val->accept(*this);
    indent--;
}

void PrintVisitor::visit(ExternalStmt &stmt) {
    printIndent();
    std::cout << "ExternalStmt: ";
    stmt.signature.accept(*this);
    std::cout << "\n";
}




void PrintVisitor::visit(DefArg &arg)  {
    printIndent();
    std::cout << "Arg: " + arg.id << "\n";
    indent++;
    arg.anno.accept(*this);
    indent--;
    std::cout << "\n";
}

void PrintVisitor::visit(FunSignature &header) {
    printIndent();
    std::cout << "fun: " << header.id << "\n";
    indent++;
    for (auto& arg: header.args) {
        arg.accept(*this);
    }
    indent--;
    printIndent();
    header.anno.accept(*this);
}

void PrintVisitor::visit(FunDef &def)  {
    printIndent();
    std::cout << "FunDef: \n";
    def.signature.accept(*this);
    printIndent();
    def.body->accept(*this);
    printIndent();
}