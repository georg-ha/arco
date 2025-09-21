#include <vector>
#include "codegen_visitor.h"
#include <llvm/IR/Module.h>

#include "expr_nodes.h"
#include "stmt_nodes.h"
#include "token_error.h"
#include "type_checker.h"

llvm::Value* CodegenVisitor::visit(const ParenExpr &expr) {
    return expr.expr->codegen_accept(*this);
}

llvm::Value* CodegenVisitor::visit(const BlockExpr &expr) {
    llvm::Value* val = nullptr;

    for (size_t i = 0; i < expr.body.size(); ++i) {
        auto& stmt = expr.body[i];

        if (i == expr.body.size() - 1) {
            if (const auto* e_s = dynamic_cast<ExprStmt*>(stmt.get())) {
                // Codegen and capture the value
                val = e_s->expr->codegen_accept(*this);
            } else {
                stmt->codegen_accept(*this);
            }
        } else {
            // Middle statements -> just codegen, discard value
            stmt->codegen_accept(*this);
        }
    }

    return val;
}

llvm::Value* CodegenVisitor::visit(const UnaryExpr &expr) {
    auto* operand_val = expr.operand->codegen_accept(*this);
    using enum UnaryOp;
    switch (expr.op) {
        case Not: return builder.CreateNeg(operand_val, "neg_tmp_bool");
        case Minus: if (expr.type == type_checker.int_ty()) {
            return builder.CreateNeg(operand_val, "neg_tmp_int");
        }
        return builder.CreateFNeg(operand_val, "neg_tmp_float");
        case Plus: return operand_val;
    }
    throw TokenError("Codegen Error!", expr.loc);
}

llvm::Value* CodegenVisitor::visit(const BinaryExpr &expr) {
    llvm::Value* left = expr.lhs->codegen_accept(*this);
    llvm::Value* right = expr.rhs->codegen_accept(*this);
    using enum BinaryOp;
    if (expr.type == type_checker.int_ty()) {
        switch (expr.op) {
            case Add: return builder.CreateAdd(left, right, "addtmp");
            case Sub: return builder.CreateSub(left, right, "subtmp");
            case Mul: return builder.CreateMul(left, right, "multmp");
            case Div: return builder.CreateSDiv(left, right, "sdivtmp");
            case Mod: return builder.CreateSRem(left, right, "sremtmp");
            default: break;
        }
    } else if (expr.type == type_checker.float_ty()) {
        switch (expr.op) {
            case Add: return builder.CreateFAdd(left, right, "faddtmp");
            case Sub: return builder.CreateFSub(left, right, "fsubtmp");
            case Mul: return builder.CreateFMul(left, right, "fmultmp");
            case Div: return builder.CreateFDiv(left, right, "fdivtmp");
            default: break;
        }
    } else if (expr.type == type_checker.bool_ty()) {
        switch (expr.op) {
            case And: return builder.CreateAnd(left, right, "andtmp");
            case Or: return builder.CreateOr(left, right, "ortmp");
            default: break;
        }
    }

    if (expr.type == type_checker.bool_ty()) {
        if (expr.lhs->type == type_checker.int_ty() && expr.rhs->type == type_checker.int_ty()) {
            switch (expr.op) {
                case Equals: return builder.CreateICmpEQ(left, right, "eqtmp");
                case NotEquals: return builder.CreateICmpNE(left, right, "netmp");
                case Greater: return builder.CreateICmpSGT(left, right, "sgttmp");
                case GreaterEquals: return builder.CreateICmpSGE(left, right, "sgetmp");
                case Less: return builder.CreateICmpSLT(left, right, "slttmp");
                case LessEquals: return builder.CreateICmpSLE(left, right, "sletmp");
                default: break;
            }
        } else if (expr.lhs->type == type_checker.float_ty() && expr.rhs->type == type_checker.float_ty()) {
            switch (expr.op) {
                case Equals: return builder.CreateFCmpOEQ(left, right, "feqtmp");
                case NotEquals: return builder.CreateFCmpONE(left, right, "fnetmp");
                case Greater: return builder.CreateFCmpOGT(left, right, "fogttmp");
                case GreaterEquals: return builder.CreateFCmpOGE(left, right, "fogetmp");
                case Less: return builder.CreateFCmpOLT(left, right, "folttmp");
                case LessEquals: return builder.CreateFCmpOLE(left, right, "foletmp");
                default: break;
            }
        }
    }
    throw TokenError("Invalid expression type", expr.loc);
}

llvm::Value* CodegenVisitor::visit(const IdExpr &expr) const {
    llvm::AllocaInst* a = expr.parent_scope->get_symbol(expr.id).val;

    return builder.CreateLoad(a->getAllocatedType(), a, expr.id.c_str());
}

llvm::Value* CodegenVisitor::visit(const IntConst& expr) const {
    return llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), expr.val, true);
}

llvm::Value* CodegenVisitor::visit(const FloatConst& expr) const {
    return llvm::ConstantFP::get(context, llvm::APFloat(expr.val));
}

llvm::Value* CodegenVisitor::visit(const CharConst &expr) const {
    return llvm::ConstantInt::get(llvm::Type::getInt8Ty(context), expr.val, true);
}

llvm::Value* CodegenVisitor::visit(const StringConst& expr) const {
    llvm::Value* str_ptr = builder.CreateGlobalString(expr.val, "str");

    return str_ptr;
}

llvm::Value* CodegenVisitor::visit(const BoolConst& expr) const {
    return llvm::ConstantInt::get(llvm::Type::getInt1Ty(context), expr.val, false);
}

llvm::Value* CodegenVisitor::visit(const FunCall &expr) {
    llvm::Function* callee = module.getFunction(expr.callee);
    if (!callee) {
        callee = get_llvm_signature(std::get<FunSymbol>(expr.parent_scope->get_symbol(expr.callee).kind).signature);
    }
    std::vector<llvm::Value*> args;
    for (const auto& arg : expr.args) {
        args.push_back(arg->codegen_accept(*this));
    }
    return builder.CreateCall(callee, args, "call_tmp");
}

llvm::Value* CodegenVisitor::visit(const IfElseExpr &expr) {
    auto cond_value = expr.condition->codegen_accept(*this);

    cond_value = builder.CreateICmpNE(cond_value,
        llvm::ConstantInt::get(cond_value->getType(), 0), "ifcond");

    llvm::Function *function = builder.GetInsertBlock()->getParent();

    llvm::BasicBlock *then_BB  = llvm::BasicBlock::Create(context, "then", function);
    llvm::BasicBlock *merge_BB = llvm::BasicBlock::Create(context, "ifcont");

    llvm::BasicBlock *else_BB = nullptr;
    if (expr.else_branch.has_value()) {
        else_BB = llvm::BasicBlock::Create(context, "else", function);
        builder.CreateCondBr(cond_value, then_BB, else_BB);
    } else {
        builder.CreateCondBr(cond_value, then_BB, merge_BB);
    }

    // then branch
    builder.SetInsertPoint(then_BB);
    llvm::Value* then_v = expr.if_branch->codegen_accept(*this);
    builder.CreateBr(merge_BB);
    then_BB = builder.GetInsertBlock();

    // else branch (only if exists)
    llvm::Value* else_v = nullptr;
    if (else_BB) {
        builder.SetInsertPoint(else_BB);
        else_v = expr.else_branch.value()->codegen_accept(*this);
        builder.CreateBr(merge_BB);
        else_BB = builder.GetInsertBlock();
    }

    // merge block
    function->insert(function->end(), merge_BB);
    builder.SetInsertPoint(merge_BB);

    llvm::PHINode* pn = builder.CreatePHI(then_v->getType(), else_v ? 2 : 1, "iftmp");
    pn->addIncoming(then_v, then_BB);
    if (else_v) pn->addIncoming(else_v, else_BB);

    return pn;
}

llvm::Value* CodegenVisitor::visit(const WhileExpr &expr) {
    llvm::Function* function = builder.GetInsertBlock()->getParent();

    llvm::BasicBlock* cond_BB = llvm::BasicBlock::Create(context, "while.cond", function);
    llvm::BasicBlock* body_BB = llvm::BasicBlock::Create(context, "while.body", function);
    llvm::BasicBlock* end_BB  = llvm::BasicBlock::Create(context, "while.end", function);


    if (!builder.GetInsertBlock()->getTerminator()) {
        builder.CreateBr(cond_BB);
    }

    builder.SetInsertPoint(cond_BB);

    llvm::Value* cond_value = expr.condition->codegen_accept(*this);

    cond_value = builder.CreateICmpNE(
        cond_value,
        llvm::ConstantInt::get(cond_value->getType(), 0),
        "whilecond"
    );


    builder.CreateCondBr(cond_value, body_BB, end_BB);

    builder.SetInsertPoint(body_BB);

    expr.body->codegen_accept(*this);

    if (!builder.GetInsertBlock()->getTerminator()) {
        builder.CreateBr(cond_BB);
    }


    builder.SetInsertPoint(end_BB);


    // While loop produces no value
    return nullptr;
}

llvm::Value* CodegenVisitor::visit(const ExprStmt& stmt) {
    return stmt.expr->codegen_accept(*this);
}
