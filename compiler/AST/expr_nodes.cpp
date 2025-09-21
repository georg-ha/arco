#include "expr_nodes.h"
#include "visitor.h"
#include "codegen_visitor.h"

void ParenExpr::accept(Visitor& visitor) { return visitor.visit(*this); }
llvm::Value* ParenExpr::codegen_accept(CodegenVisitor& visitor)  { return visitor.visit(*this); }

std::string str_of_unary_op(UnaryOp op) {
    using enum UnaryOp;
    switch (op) {
        case Minus: return "-";
        case Plus: return "+";
        case Not:   return  "not";
    }
    return "";
}

void IdExpr::accept(Visitor& visitor)  { return visitor.visit(*this); }
llvm::Value* IdExpr::codegen_accept(CodegenVisitor& visitor)  { return visitor.visit(*this); }

void FunCall::accept(Visitor& visitor)  { return visitor.visit(*this); }
llvm::Value* FunCall::codegen_accept(CodegenVisitor& visitor)  { return visitor.visit(*this); }

void UnaryExpr::accept(Visitor& visitor)  { return visitor.visit(*this); }
llvm::Value* UnaryExpr::codegen_accept(CodegenVisitor& visitor)  { return visitor.visit(*this); }

std::string str_of_binary_op(BinaryOp op) {
    using enum BinaryOp;
    switch (op) {
        case Add: return "+";
        case Sub: return "-";
        case Mul: return "+";
        case Div: return "/";
        case Mod: return "%";
        case Concat: return "^";
        case Equals: return "==";
        case NotEquals: return "!=";
        case Less: return "<";
        case Greater: return ">";
        case LessEquals: return "<=";
        case GreaterEquals: return ">=";
        case And: return "and";
        case Or: return "or";
    }
    return "";
}

void BinaryExpr::accept(Visitor& visitor)  { return visitor.visit(*this); }
llvm::Value* BinaryExpr::codegen_accept(CodegenVisitor& visitor)  { return visitor.visit(*this); }

void BlockExpr::accept(Visitor& visitor) { return visitor.visit(*this); }
llvm::Value* BlockExpr::codegen_accept(CodegenVisitor& visitor)  { return visitor.visit(*this); }

void IfElseExpr::accept(Visitor& visitor) { return visitor.visit(*this); }
llvm::Value* IfElseExpr::codegen_accept(CodegenVisitor& visitor)  { return visitor.visit(*this); }

void WhileExpr::accept(Visitor &visitor) { return visitor.visit(*this); }
llvm::Value* WhileExpr::codegen_accept(CodegenVisitor& visitor)  { return visitor.visit(*this); }

void IntConst::accept(Visitor& visitor)  { return visitor.visit(*this); }
llvm::Value* IntConst::codegen_accept(CodegenVisitor& visitor)  { return visitor.visit(*this); }

void FloatConst::accept(Visitor& visitor)  { return visitor.visit(*this); }
llvm::Value* FloatConst::codegen_accept(CodegenVisitor& visitor)  { return visitor.visit(*this); }

void CharConst::accept(Visitor& visitor)  { return visitor.visit(*this); }
llvm::Value* CharConst::codegen_accept(CodegenVisitor& visitor)  { return visitor.visit(*this); }

void StringConst::accept(Visitor& visitor)  { return visitor.visit(*this); }
llvm::Value* StringConst::codegen_accept(CodegenVisitor& visitor)  { return visitor.visit(*this); }

void BoolConst::accept(Visitor& visitor)  { return visitor.visit(*this); }
llvm::Value* BoolConst::codegen_accept(CodegenVisitor& visitor)  { return visitor.visit(*this); }
